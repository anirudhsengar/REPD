// --- CacheManager.cpp ---
// A generic manager for caching resources of a given type.
// It uses raw pointers for demonstration purposes, simulating a legacy system 
// or one that needs to interface with a C-style library.

#include <iostream>
#include <string>
#include <map>
#include <stdexcept>

template<typename T>
class ResourceCache {
public:
    // Constructor
    ResourceCache(const std::string& name) : m_cacheName(name) {
        std::cout << "Creating cache: " << m_cacheName << std::endl;
    }

    // Destructor
    ~ResourceCache() {
        std::cout << "Destroying cache: " << m_cacheName << std::endl;
        for (auto const& [key, val] : m_resourceMap) {
            delete val;
        }
        m_resourceMap.clear();
    }

    // Add a new resource to the cache
    void addResource(const std::string& key, T* resource) {
        if (m_resourceMap.find(key) != m_resourceMap.end()) {
            // Free existing resource before overwriting
            delete m_resourceMap[key];
        }
        m_resourceMap[key] = resource;
    }

    // Get a resource by its key
    T* getResource(const std::string& key) const {
        auto it = m_resourceMap.find(key);
        if (it == m_resourceMap.end()) {
            throw std::runtime_error("Resource not found in cache: " + key);
        }
        return it->second;
    }

    // Get the number of items in the cache
    size_t size() const {
        return m_resourceMap.size();
    }

private:
    std::string m_cacheName;
    std::map<std::string, T*> m_resourceMap;
    // Missing copy constructor and copy assignment operator
};

// Example usage structure (not required for bug)
struct Texture {
    int id;
    std::string path;
};

void processCaches() {
    ResourceCache<Texture> originalCache("OriginalTextures");
    originalCache.addResource("sky", new Texture{101, "assets/sky.png"});
    originalCache.addResource("grass", new Texture{102, "assets/grass.png"});

    // Problem occurs here
    ResourceCache<Texture> copiedCache = originalCache; // Shallow copy
    copiedCache.addResource("stone", new Texture{103, "assets/stone.png"});

} // originalCache and copiedCache go out of scope here