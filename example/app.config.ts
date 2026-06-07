import type {
	ExpoConfig,
} from "@expo/config"

const
	packageName =
		"id.sufeni.oss.reactnativeuws.example",

	versionCode =
		1,

	versionName =
		"1.0.0"

export default {

	name: "UWS Example",
	slug: "uws",
	scheme: "uws",
	version: versionName,

	orientation: "default",

	platforms: ["android"],

	userInterfaceStyle: "automatic",

	android: {
		versionCode: versionCode,
		adaptiveIcon: {
			backgroundColor: "#ffffff",
			foregroundImage: "./assets/android-icon-foreground.png",
			backgroundImage: "./assets/android-icon-background.png",
			monochromeImage: "./assets/android-icon-monochrome.png",
		},
		package: packageName,
	},

	ios: {
		buildNumber: versionCode.toString(),
		supportsTablet: true,
		bundleIdentifier: packageName,
		icon: "./assets/android-icon-foreground.png",
		config: {
			usesNonExemptEncryption: false,
		},
	},

	plugins: [
		[
			"expo-build-properties",
			{
				android: {
					minSdkVersion: 28,
					packagingOptions: {
						merge: ["META-INF/*"],
					},
				},
				ios: {
					deploymentTarget: "26.0",
				},
			},
		],
		[
			"expo-image-picker",
			{
				photosPermission: "The app accesses your photos to build Material Color from the selected photo.",
			},
		],
		[
			"expo-splash-screen",
			{
				image: "./assets/splash-icon.png",
				imageWidth: 200,
				resizeMode: "contain",
				backgroundColor: "#ffffff",
				dark: {
					backgroundColor: "#ffffff",
				},
			},
		],
	],

	experiments: {
		autolinkingModuleResolution: true,
		typedRoutes: true,
		reactCompiler: false,
	},

} satisfies ExpoConfig
