// --- CacheManager_Fixed.cpp ---
// A generic, modern, and safe manager for caching resources.
// It uses std::unique_ptr to handle memory ownership automatically,
// preventing memory leaks and double-frees.

#include <iostream>
#include <string>
#include <map>
#include <memory> // Required for std::unique_ptr
#include <stdexcept>

template<typename T>
class ResourceCache {
public:
    // Constructor
    ResourceCache(const std::string& name) : m_cacheName(name) {
        std::cout << "Creating cache: " << m_cacheName << std::endl;
    }

    // No explicit destructor needed! The unique_ptrs will automatically
    // delete the managed objects when the map is destroyed. This is the Rule of Zero.

    // Add a new resource to the cache.
    // The cache takes ownership of the pointer.
    void addResource(const std::string& key, std::unique_ptr<T> resource) {
        // std::map::insert_or_assign is a clean way to handle this.
        // If the key exists, the old unique_ptr is destroyed (deleting its object)
        // before the new one is moved into place.
        m_resourceMap.insert_or_assign(key, std::move(resource));
    }

    // Get a non-owning raw pointer to a resource.
    // The caller should not delete this pointer.
    T* getResource(const std::string& key) const {
        auto it = m_resourceMap.find(key);
        if (it == m_resourceMap.end()) {
            throw std::runtime_error("Resource not found in cache: " + key);
        }
        // .get() returns the raw pointer managed by the unique_ptr.
        return it->second.get();
    }

    // Get the number of items in the cache.
    size_t size() const {
        return m_resourceMap.size();
    }

    // The compiler-generated copy constructor and assignment operator are deleted
    // because std::unique_ptr is not copyable, which is the correct and safe
    // default behavior for a class that exclusively owns resources.
    // We could implement move semantics if needed, but for now this is safest.
    ResourceCache(const ResourceCache&) = delete;
    ResourceCache& operator=(const ResourceCache&) = delete;

private:
    std::string m_cacheName;
    std::map<std::string, std::unique_ptr<T>> m_resourceMap;
};

// Example usage structure
struct Texture {
    int id;
    std::string path;
    ~Texture() { std::cout << "Destroying texture: " << path << std::endl; }
};

void processCachesSafely() {
    ResourceCache<Texture> textureCache("GlobalTextures");
    textureCache.addResource("sky", std::make_unique<Texture>(Texture{101, "assets/sky.png"}));
    textureCache.addResource("grass", std::make_unique<Texture>(Texture{102, "assets/grass.png"}));
    
    // Attempting to copy will now result in a compile-time error, which is good.
    // ResourceCache<Texture> copiedCache = textureCache; // COMPILE ERROR

} // textureCache goes out of scope here. Its destructor is called, which in turn
  // calls the destructors for all the std::unique_ptr objects in the map,
  // safely deleting the Texture objects.