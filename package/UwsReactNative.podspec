require "json"

package = JSON.parse(File.read(File.join(__dir__, "package.json")))

Pod::Spec.new do |s|
  s.name         = "UwsReactNative"
  s.version      = package["version"]
  s.summary      = package["description"]
  s.homepage     = package["homepage"]
  s.license      = package["license"]
  s.authors      = package["author"]

  s.platforms    = { :ios => min_ios_version_supported, :osx => "10.14" }
  s.source       = { :git => package["repository"]["url"], :tag => "#{s.version}" }

  s.source_files          = "apple/**/*.{h,m,mm,cpp}", "cpp/*.{cpp,h}"
  s.ios.exclude_files     = "**/*.macos.{h,m,mm}"
  s.tvos.exclude_files    = "**/*.macos.{h,m,mm}"
  s.osx.exclude_files     = "**/*.ios.{h,m,mm}"
  s.private_header_files  = "apple/**/*.h"

  # zlib
  s.libraries             = "z"

  s.pod_target_xcconfig = {
    # Fix IPV6_RECVPKTINFO undeclared for uSockets
    "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) __APPLE_USE_RFC_3542"
  }

  s.subspec "app" do |ss|
    ss.source_files         = "cpp/app/**/*.h"
    ss.project_header_files = "cpp/app/**/*.h"
    ss.header_mappings_dir  = "cpp"
  end

  s.subspec "jsi" do |ss|
    ss.source_files         = "cpp/jsi/**/*.h"
    ss.project_header_files = "cpp/jsi/**/*.h"
    ss.header_mappings_dir  = "cpp"
  end

  s.subspec "uSockets" do |ss|
    ss.source_files         = "cpp/uSockets/**/*.{c,cpp,h}"
  
    # Same like the cpp/CMakeLists.txt
    # whoever want to use uSockets including the uWebSockets
    # They just need to write like ` #include "libusockets.h" `.
    ss.header_mappings_dir  = "cpp/uSockets"
  
    ss.pod_target_xcconfig = {
      "GCC_PREPROCESSOR_DEFINITIONS" => "$(inherited) LIBUS_NO_SSL"
    }
  end

  s.subspec "uWebSockets" do |ss|
    ss.source_files         = "cpp/uWebSockets/uWebSockets/*.h"
    ss.project_header_files = "cpp/uWebSockets/uWebSockets/*.h"
    ss.header_mappings_dir  = "cpp/uWebSockets"
  end

  s.dependency "React-jsi"
  s.dependency "React-callinvoker"

  install_modules_dependencies(s)
end
