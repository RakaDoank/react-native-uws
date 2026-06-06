const
	node_path =
		require("node:path"),

	{
		getDefaultConfig,
	} =
		require("expo/metro-config"),

	{
		mergeConfig,
	} =
		require("@expo/metro/metro-config")

const
	projectRoot =
		__dirname,

	workspaceRoot =
		node_path.resolve(projectRoot, ".."),

	defaultConfig =
		getDefaultConfig(__dirname)

/**
 * Metro configuration
 * https://reactnative.dev/docs/metro
 *
 * @type {import('@expo/metro-config').MetroConfig}
 */
const config = {

	projectRoot,

	resolver: {
		...defaultConfig.resolver,
		extraNodeModules: {
			...(defaultConfig.resolver?.extraNodeModules ?? {}),
			"react-native-uws": node_path.join(workspaceRoot, "package", "src"),
		},
	},

};

module.exports = mergeConfig(defaultConfig, config);
