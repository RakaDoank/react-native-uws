/**
 * @type {import("@react-native-community/cli-types").UserDependencyConfig}
 */
module.exports = {
	dependency: {
		platforms: {
			android: {
				cmakeListsPath: "build/generated/source/codegen/jni/CMakeLists.txt",

				// our CMake project name
				cxxModuleCMakeListsModuleName: "react-native-uws",

				// relative from android directory
				cxxModuleCMakeListsPath: "../cpp/CMakeLists.txt",

				cxxModuleHeaderName: "ReactNativeUwsModule",
			},
		},
	},
}
