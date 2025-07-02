// --- SettingsManager.cpp ---
// A singleton manager for global application settings.
// It loads settings from a file and provides access to them.

#include <string>
#include <vector>
#include <algorithm>
#include <stdexcept>

struct Setting {
    std::string key;
    int value;
};

class SettingsManager {
public:
    static SettingsManager& getInstance() {
        static SettingsManager instance;
        return instance;
    }

    // Load settings from a source (e.g., a file)
    void load() {
        // In a real app, this would read from a file.
        m_settings.push_back({"difficulty", 5});
        m_settings.push_back({"volume", 75});
        m_settings.push_back({"render_distance", 12});
    }

    // Get a setting's value by key.
    // Returns a reference to the value for modification.
    const int& getSettingValue(const std::string& key) const {
        auto it = std::find_if(m_settings.begin(), m_settings.end(), 
                               [&](const Setting& s) { return s.key == key; });

        if (it != m_settings.end()) {
            return it->value;
        }

        // If not found, return a default value.
        // THIS IS THE BUG.
        const int defaultValue = 0;
        return defaultValue;
    }

private:
    SettingsManager() = default;
    ~SettingsManager() = default;
    SettingsManager(const SettingsManager&) = delete;
    void operator=(const SettingsManager&) = delete;

    std::vector<Setting> m_settings;
};

void applySettings() {
    SettingsManager::getInstance().load();

    // This call is fine.
    const int& volume = SettingsManager::getInstance().getSettingValue("volume");
    // std::cout << "Volume is: " << volume << std::endl; // Would print 75

    // This call triggers the bug.
    const int& nonExistent = SettingsManager::getInstance().getSettingValue("vsync");
    
    // The memory for `defaultValue` in getSettingValue is gone. `nonExistent` is a dangling reference.
    // Using it here is undefined behavior. It might print 0, or garbage, or crash.
    // std::cout << "VSync is: " << nonExistent << std::endl;
}