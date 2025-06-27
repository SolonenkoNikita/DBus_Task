#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include <JsonConfigFileManager/JsonConfigFileManager.hpp>

namespace fs = std::filesystem;

class JsonConfigFileManagerTest : public ::testing::Test 
{
protected:
    void SetUp() override 
    {
        temp_dir = fs::temp_directory_path() / "config_manager_test";
        fs::create_directory(temp_dir);
    }

    void TearDown() override 
    {
        fs::remove_all(temp_dir);
    }

    fs::path temp_dir;
};

TEST_F(JsonConfigFileManagerTest, LoadValidConfig) 
{
    const auto config_path = temp_dir / "valid_config.json";
    std::ofstream(config_path) << R"({
        "Timeout": 1000,
        "TimeoutPhrase": "Hello"
    })";

    JsonConfigFileManager manager;
    auto config = manager.load(config_path.string());

    EXPECT_EQ(config["Timeout"].get<uint32_t>(), 1000);
    EXPECT_EQ(config["TimeoutPhrase"].get<std::string>(), "Hello");
}

TEST_F(JsonConfigFileManagerTest, LoadNonExistentFile) 
{
    JsonConfigFileManager manager;
    EXPECT_THROW(manager.load("/nonexistent/file.json"), std::runtime_error);
}

TEST_F(JsonConfigFileManagerTest, LoadInvalidJson) 
{
    const auto config_path = temp_dir / "invalid.json";
    std::ofstream(config_path) << "{ invalid: json }";

    JsonConfigFileManager manager;
    EXPECT_THROW(manager.load(config_path.string()), std::runtime_error);
}

TEST_F(JsonConfigFileManagerTest, LoadMissingTimeout) 
{
    const auto config_path = temp_dir / "missing_timeout.json";
    std::ofstream(config_path) << R"({
        "TimeoutPhrase": "Hello"
    })";

    JsonConfigFileManager manager;
    EXPECT_THROW(manager.load(config_path.string()), std::runtime_error);
}

TEST_F(JsonConfigFileManagerTest, LoadInvalidTimeoutType) 
{
    const auto config_path = temp_dir / "invalid_timeout.json";
    std::ofstream(config_path) << R"({
        "Timeout": "not_a_number",
        "TimeoutPhrase": "Hello"
    })";

    JsonConfigFileManager manager;
    EXPECT_THROW(manager.load(config_path.string()), std::runtime_error);
}

TEST_F(JsonConfigFileManagerTest, SaveConfig) 
{
    const auto config_path = temp_dir / "saved_config.json";
    JsonConfigFileManager manager;

    std::map<std::string, sdbus::Variant> config = 
    {
        {"Timeout", sdbus::Variant{2000}},
        {"TimeoutPhrase", sdbus::Variant{"New phrase"}}
    };

    manager.save(config_path.string(), config);

    std::ifstream file(config_path);
    nlohmann::json j;
    file >> j;

    EXPECT_EQ(j["Timeout"], 2000);
    EXPECT_EQ(j["TimeoutPhrase"], "New phrase");
}