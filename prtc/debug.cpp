#include <iostream>
#include <dlfcn.h>
#include <string>
#include <functional>
#include <memory>
#include <optional>

class DynamicLibrary : public std::enable_shared_from_this<DynamicLibrary> {
 public:
  static std::shared_ptr<DynamicLibrary> open(const std::string& path){
    return std::make_shared<DynamicLibrary>(path);
  };

  explicit DynamicLibrary(const std::string& path)
  : path_{path}, handle_{dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL)} {
  if (!handle_) {
    char* dlerror_message = dlerror();
    std::string error_message =
        "DynamicLibrary: failed to load " + path + "\n" + dlerror_message;
    throw std::runtime_error(error_message);
    }
    std::cout<<" Module created from path: "<<path<<std::endl;
    std::cout << " Dynamic Library created..\n";
  };

  ~DynamicLibrary() { dlclose(handle_); };

  std::shared_ptr<DynamicLibrary> share() {
    return shared_from_this();
  };

  void * getFunction(const std::string& name) {
    return getSymbol(name);
  }

 private:
  void* getSymbol(const std::string& name) const {
  std::cout<<"\n Entered GetSymbol\n";
  std::cout<<"Looking for symbol " << name << " in path: " << path_ <<"\n";
  void* symbol = dlsym(handle_, name.c_str());
  std::cout<<"\n Received Symbol\n";
  if (!symbol) {
    char* dlerror_message = dlerror();
    std::string error_message = "DynamicLibrary: failed to find symbol " +
                                name + " in " + path_ + "\n" + dlerror_message;
    throw std::runtime_error(error_message);
  }
  return symbol;
  };

  std::string path_;
  void* handle_;
};

using Kernel_t = std::function<void(void*, int, float)>;
using Module_t = std::shared_ptr<DynamicLibrary>;

void LoadModule(const std::string& path, Module_t *module) {
    *module = DynamicLibrary::open(path);
}

void GetKernel(Module_t module, std::string kernel_name, void* kernel) {
    void* kernel_ptr = module->getFunction(kernel_name);
    kernel = kernel_ptr;
}

int main() {
    std::cout<<"\n Starting debugging\n";
    std::string binary_name = "./binary.so";
    std::string kernel_name = "CeedKernelSyclRefQFunction_setup";

    // void* handle = dlopen(binary_name.c_str(), RTLD_NOW | RTLD_LOCAL);
    // std::cout<<" Opened binary file\n";
    // void *symbol = dlsym(handle,kernel_name.c_str());
    // std::cout<<" Retrieved kernel pointer\n";
    // Kernel_t *kernel = reinterpret_cast<Kernel_t*>(symbol);
    // std::cout<<" Kernel pointer recast\n";

    std::cout<<"\nApplication case:\n";
    // Module_t module = DynamicLibrary::open("./binary.so");
    Module_t module;
    LoadModule(binary_name, &module);
    std::cout<<"Opened binary file\n";
    void *kernel_ptr;
    GetKernel(module, kernel_name, kernel_ptr);
    // void *kernel_ptr = module->getFunction(kernel_name);
    std::cout<<" Retrieved kernel pointer\n";

    return 1;
}