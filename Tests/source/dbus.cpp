#include <gtest/gtest.h>

#include <sdbus-c++/sdbus-c++.h>
#include <thread>
#include <chrono>
#include <future>

#include <AppConfig/AppConfig.hpp>
#include <DBusConfigAdapter/DBusConfigAdapter.hpp>

class MockConfigStorage : public IConfigStorage 
{
public:
    MockConfigStorage(std::string app_name, std::map<std::string, sdbus::Variant> params = {})
        : app_name_(std::move(app_name)), parameters_(std::move(params)) {}

    std::map<std::string, sdbus::Variant> getAllParameters() const override 
    {
        return parameters_;
    }
    
    void setParameter(const std::string& key, const sdbus::Variant& value) override 
    {
        if (key == "throw") 
            throw std::runtime_error("Test error");
        parameters_[key] = value;
    }
    
    std::string getAppName() const override 
    {
        return app_name_;
    }

    void setParameters(const std::map<std::string, sdbus::Variant>& params) 
    {
        parameters_ = params;
    }

private:
    std::string app_name_;
    std::map<std::string, sdbus::Variant> parameters_;
};

class DBusConfigAdapterTest : public ::testing::Test 
{
protected:
    static void SetUpTestSuite() 
    {
        main_loop_thread_ = std::thread([]
        {
            connection_ = sdbus::createSessionBusConnection("test.config.manager");
            connection_->enterEventLoop();
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    static void TearDownTestSuite() 
    {
        if (connection_) 
            connection_->leaveEventLoop();
        if (main_loop_thread_.joinable()) 
            main_loop_thread_.join();
        connection_.reset();
    }

    void SetUp() override 
    {
        mock_storage_ = std::make_unique<MockConfigStorage>("testApp");
        adapter_ = std::make_unique<DBusConfigAdapter>(std::move(mock_storage_), *connection_);
        adapter_->registerDBusInterface();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void TearDown() override 
    {
        adapter_.reset();
    }

    static std::unique_ptr<sdbus::IConnection> connection_;
    static std::thread main_loop_thread_;
    
    std::unique_ptr<MockConfigStorage> mock_storage_;
    std::unique_ptr<DBusConfigAdapter> adapter_;
};

std::unique_ptr<sdbus::IConnection> DBusConfigAdapterTest::connection_;
std::thread DBusConfigAdapterTest::main_loop_thread_;

TEST_F(DBusConfigAdapterTest, GetConfigurationReturnsEmptyForNewStorage)
{
    auto proxy = sdbus::createProxy(*connection_, "test.config.manager",
        "/com/system/configurationManager/Application/testApp");
    
    for (size_t i = 0; i < 3; ++i) 
    {
        try 
        {
            std::map<std::string, sdbus::Variant> result;
            proxy->callMethod("GetConfiguration")
                .onInterface("com.system.configurationManager.Application.Configuration")
                .withTimeout(std::chrono::milliseconds(500))
                .storeResultsTo(result);
            
            EXPECT_TRUE(result.empty());
            return; 
        } 
        catch (const sdbus::Error&) 
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    FAIL() << "Failed after 3 attempts";
}

TEST_F(DBusConfigAdapterTest, ChangeConfigurationWorks) 
{
    auto proxy = sdbus::createProxy(*connection_, "test.config.manager",
        "/com/system/configurationManager/Application/testApp");
    
    proxy->callMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withArguments("testKey", sdbus::Variant(42))
        .withTimeout(std::chrono::milliseconds(500));
    
    std::map<std::string, sdbus::Variant> result;
    proxy->callMethod("GetConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withTimeout(std::chrono::milliseconds(500))
        .storeResultsTo(result);
    
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result["testKey"].get<int32_t>(), 42);
}

TEST_F(DBusConfigAdapterTest, EmitsConfigurationChangedSignal) 
{
    auto proxy = sdbus::createProxy(*connection_, "test.config.manager",
        "/com/system/configurationManager/Application/testApp");
    
    std::promise<void> signal_promise;
    auto signal_future = signal_promise.get_future();
    
    proxy->uponSignal("configurationChanged")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .call([&](const std::map<std::string, sdbus::Variant>& config) 
        {
            EXPECT_EQ(config.size(), 1);
            EXPECT_EQ(config.at("signalTest").get<int32_t>(), 123);
            signal_promise.set_value();
        });
    proxy->finishRegistration();
    
    proxy->callMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withArguments("signalTest", sdbus::Variant(123))
        .withTimeout(std::chrono::milliseconds(500));
    
    EXPECT_EQ(signal_future.wait_for(std::chrono::seconds(1)), std::future_status::ready);
}

TEST_F(DBusConfigAdapterTest, HandlesDifferentVariantTypes) 
{
    auto proxy = sdbus::createProxy(*connection_, "test.config.manager",
        "/com/system/configurationManager/Application/testApp");
    
    proxy->callMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withArguments("intVal", sdbus::Variant(int32_t{42}));
    
    proxy->callMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withArguments("strVal", sdbus::Variant("test string"));
    
    proxy->callMethod("ChangeConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .withArguments("boolVal", sdbus::Variant(true));
    
    std::map<std::string, sdbus::Variant> result;
    proxy->callMethod("GetConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .storeResultsTo(result);
    
    EXPECT_EQ(result["intVal"].get<int32_t>(), 42);
    EXPECT_EQ(result["strVal"].get<std::string>(), "test string");
    EXPECT_EQ(result["boolVal"].get<bool>(), true);
}

TEST_F(DBusConfigAdapterTest, ConcurrentAccess) 
{
    auto proxy = sdbus::createProxy(*connection_, "test.config.manager",
        "/com/system/configurationManager/Application/testApp");
    
    constexpr int THREAD_COUNT = 5;
    constexpr int ITERATIONS = 20;
    
    std::mutex mutex;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < THREAD_COUNT; ++i) 
    {
        threads.emplace_back([&proxy, i, &mutex] 
        {
            for (size_t j = 0; j < static_cast<size_t>(ITERATIONS); ++j) 
            {
                std::string key = "thread_" + std::to_string(i) + "_" + std::to_string(j);
                try 
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    proxy->callMethod("ChangeConfiguration")
                        .onInterface("com.system.configurationManager.Application.Configuration")
                        .withArguments(key, sdbus::Variant(j))
                        .withTimeout(std::chrono::milliseconds(100));
                } 
                catch (...) 
                {
                }
            }
        });
    }
    
    for (auto& t : threads) 
        t.join();
    
    std::map<std::string, sdbus::Variant> result;
    proxy->callMethod("GetConfiguration")
        .onInterface("com.system.configurationManager.Application.Configuration")
        .storeResultsTo(result);
    
    EXPECT_EQ(result.size(), THREAD_COUNT * ITERATIONS);
}