#include <gtest/gtest.h>
#include <sdbus-c++/sdbus-c++.h>

#include <AppConfig/AppConfig.hpp>
#include <thread>

class AppConfigTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        test_config = {{"Timeout", sdbus::Variant(uint32_t{1000})},
                       {"TimeoutPhrase", sdbus::Variant(std::string{"Test"})},
                       {"DebugMode", sdbus::Variant(true)}};
    }

    std::map<std::string, sdbus::Variant> test_config;
};

TEST_F(AppConfigTest, ConstructorInitializesValues)
{
    AppConfig config("testApp", test_config);

    EXPECT_EQ(config.getAppName(), "testApp");
    auto params = config.getAllParameters();
    EXPECT_EQ(params.size(), 3);
}

TEST_F(AppConfigTest, GetAppNameReturnsCorrectValue)
{
    AppConfig config("weatherApp", test_config);
    EXPECT_EQ(config.getAppName(), "weatherApp");
}

TEST_F(AppConfigTest, GetAllParametersReturnsCompleteConfig)
{
    AppConfig config("testApp", test_config);
    auto params = config.getAllParameters();

    EXPECT_EQ(params["Timeout"].get<uint32_t>(), 1000);
    EXPECT_EQ(params["TimeoutPhrase"].get<std::string>(), "Test");
    EXPECT_EQ(params["DebugMode"].get<bool>(), true);
}

TEST_F(AppConfigTest, SetParameterUpdatesExistingValue)
{
    AppConfig config("testApp", test_config);

    config.setParameter("Timeout", sdbus::Variant(uint32_t{2000}));

    auto params = config.getAllParameters();
    EXPECT_EQ(params["Timeout"].get<uint32_t>(), 2000);
}

TEST_F(AppConfigTest, SetParameterAddsNewValue)
{
    AppConfig config("testApp", test_config);

    config.setParameter("RetryCount", sdbus::Variant(uint32_t{3}));

    auto params = config.getAllParameters();
    EXPECT_EQ(params.size(), 4);
    EXPECT_EQ(params["RetryCount"].get<uint32_t>(), 3);
}

TEST_F(AppConfigTest, ThreadSafetyCheck)
{
    AppConfig config("testApp", test_config);
    const int iterations = 1000;
    std::atomic<bool> writers_done{false};
    std::atomic<int> read_errors{0};
    std::atomic<int> last_value{0};

    auto writer_fn = [&config, iterations]()
    {
        for (int i = 0; i < iterations; ++i) config.setParameter("Counter", sdbus::Variant(i));
    };

    auto reader_fn = [&config, &writers_done, &read_errors, &last_value]()
    {
        while (!writers_done)
        {
            int val = config.getAllParameters()["Counter"].get<int>();
            last_value = val;

            if (val < 0)
                ++read_errors;
        }
    };

    std::thread writer1(writer_fn);
    std::thread writer2(writer_fn);
    std::thread reader(reader_fn);

    writer1.join();
    writer2.join();
    writers_done = true;
    reader.join();

    EXPECT_EQ(read_errors.load(), 0);

    EXPECT_GE(last_value.load(), 0);
    EXPECT_LT(last_value.load(), iterations);
}

TEST_F(AppConfigTest, HandlesDifferentVariantTypes)
{
    AppConfig config("testApp", {});

    config.setParameter("IntValue", sdbus::Variant(int32_t{-42}));
    config.setParameter("UIntValue", sdbus::Variant(uint32_t{42}));
    config.setParameter("StringValue", sdbus::Variant(std::string{"Hello"}));
    config.setParameter("BoolValue", sdbus::Variant(true));

    auto params = config.getAllParameters();
    EXPECT_EQ(params["IntValue"].get<int32_t>(), -42);
    EXPECT_EQ(params["UIntValue"].get<uint32_t>(), 42);
    EXPECT_EQ(params["StringValue"].get<std::string>(), "Hello");
    EXPECT_EQ(params["BoolValue"].get<bool>(), true);
}
