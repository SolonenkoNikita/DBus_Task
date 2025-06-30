#include <gtest/gtest.h>

#include <ConfigurationManager/ConfigurationManager.hpp>
#include <filesystem>
#include <fstream>

class MockConfigFileManager : public IConfigFileManager
{
   public:
    using LoadFunc = std::function<std::map<std::string, sdbus::Variant>(const std::string&)>;

    void setMockLoad(LoadFunc func) { loadFunc_ = std::move(func); }

    std::map<std::string, sdbus::Variant> load(const std::string& path) override
    {
        if (loadFunc_)
            return loadFunc_(path);
        return {};
    }

    void save(const std::string&, const std::map<std::string, sdbus::Variant>&) override {}

   private:
    LoadFunc loadFunc_;
};

namespace fs = std::filesystem;

class ConfigurationManagerTest : public ::testing::Test
{
   protected:
    void SetUp() override
    {
        temp_dir_ = fs::temp_directory_path() / "config_manager_test";
        fs::create_directories(temp_dir_);

        mock_loader_ = std::make_unique<MockConfigFileManager>();
    }

    void TearDown() override { fs::remove_all(temp_dir_); }

    void createTestConfig(const std::string& filename, const std::string& content)
    {
        std::ofstream f(temp_dir_ / filename);
        f << content;
        f.close();
    }

    fs::path temp_dir_;
    std::unique_ptr<MockConfigFileManager> mock_loader_;
};

TEST_F(ConfigurationManagerTest, CreatesDefaultConfigDirectory)
{
    fs::remove_all(temp_dir_);

    ConfigurationManager manager(std::move(mock_loader_), temp_dir_.string());
    EXPECT_NO_THROW(manager.run());
    EXPECT_TRUE(fs::exists(temp_dir_));
}

TEST_F(ConfigurationManagerTest, LoadsValidConfigFiles)
{
    createTestConfig("app1.json", R"({"param1": 123})");
    createTestConfig("app2.conf", R"({"param2": "value"})");
    createTestConfig("invalid.txt", "not a config");

    mock_loader_->setMockLoad([](const std::string&)
                              { return std::map<std::string, sdbus::Variant>{{"test", sdbus::Variant(1)}}; });

    ConfigurationManager manager(std::move(mock_loader_), temp_dir_.string());
    manager.run();
}

TEST_F(ConfigurationManagerTest, UsesCustomConfigDirectory)
{
    const std::string custom_dir = (fs::temp_directory_path() / "custom_config").string();
    ConfigurationManager manager(std::make_unique<MockConfigFileManager>(), custom_dir);

    EXPECT_NO_THROW(manager.run());
    EXPECT_TRUE(fs::exists(custom_dir));
    fs::remove_all(custom_dir);
}
