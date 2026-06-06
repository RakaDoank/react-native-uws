buildscript {
  val module by extra(
    mapOf(
      "kotlinVersion" to "2.0.21",
      "minSdkVersion" to 28,
      "compileSdkVersion" to 36,
      "targetSdkVersion" to 36,
    )
  )

  val getExtOrDefault by extra {
    { name: String ->
      if(rootProject.extra.has(name)) {
        rootProject.extra[name]!!
      } else {
        module[name]!!
      }
    }
  }

  repositories {
    google()
    mavenCentral()
  }

  dependencies {
    classpath("com.android.tools.build:gradle:8.7.2")

    // noinspection DifferentKotlinGradleVersion
    classpath("org.jetbrains.kotlin:kotlin-gradle-plugin:${getExtOrDefault("kotlinVersion")}")
  }
}

plugins {
  id("com.android.library")
  id("org.jetbrains.kotlin.android")

  id("com.facebook.react")
}

android {
  val getExtOrDefault = extra["getExtOrDefault"] as (String) -> Any

  namespace = "id.sufeni.oss.reactnativeuws"

  compileSdk = getExtOrDefault("compileSdkVersion") as Int

  defaultConfig {
    minSdk = getExtOrDefault("minSdkVersion") as Int

    // https://issuetracker.google.com/issues/230625468?pli=1
    // It is not useful for libraries to declare a targetSdkVersion and soon the manifest merger will ignore it.
    // targetSdk = getExtOrDefault("targetSdkVersion") as Int

    externalNativeBuild {
      cmake {
        cppFlags += listOf(
          "-frtti",
          "-fexceptions",
          "-Wall",
          "-fstack-protector-all",
        )
        arguments += listOf(
          "-DANDROID_STL=c++_shared",
          "-DANDROID_SUPPORT_FLEXIBLE_PAGE_SIZES=ON",
        )

        buildTypes {
          debug {
            cppFlags += "-01 -g"
          }
          release {
            cppFlags += "-02"
          }
        }
      }
    }
  }

  // We don't need this, because the current CMakeLists.txt is
  // already picked and linked by React Native (app level) to build C++ Turbo Module
  // with an help by our `dependency.platform.android.cxxModuleCMakeListsPath` option in `react-native.config.js`.
  // We may only need this for custom JNI
//  externalNativeBuild {
//    cmake {
//      path("CMakeLists.txt")
//    }
//  }

  packaging {
    resources {
      excludes += listOf(
        "META-INF",
        "META-INF/**",
        "**/libjsi.so",
        "**/libc++_shared.so",
        "**/libreact_render*.so",
        "**/libreactnativejni.so",
        "**/libreact_performance_timeline.so",
        // In 0.76 multiple react-native's libraries were merged and these are the main new artifacts we're using.
        // Some of above lib* names could be removed after we remove support for 0.76.
        // https://github.com/facebook/react-native/pull/43909
        // https://github.com/facebook/react-native/pull/46059
        "**/libfbjni.so",
        "**/libreactnative.so"
      )
    }
  }

  buildFeatures {
    buildConfig = true
    prefab = true
  }

  buildTypes {
    debug {
      isJniDebuggable = true
    }
    release {
      isMinifyEnabled = false
    }
  }

  lint {
    disable += "GradleCompatible"
    targetSdk = getExtOrDefault("targetSdkVersion") as Int
  }

  compileOptions {
    sourceCompatibility = JavaVersion.VERSION_17
    targetCompatibility = JavaVersion.VERSION_17
  }
}

dependencies {
  val getExtOrDefault = project.extra["getExtOrDefault"] as (String) -> Any
  val kotlinVersion = getExtOrDefault("kotlinVersion") as String

  implementation("com.facebook.react:react-android")
  implementation("org.jetbrains.kotlin:kotlin-stdlib:$kotlinVersion")
}
