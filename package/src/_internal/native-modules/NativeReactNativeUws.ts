import {
	TurboModuleRegistry,
	type CodegenTypes,
	type TurboModule,
} from "react-native"

/**
 * This spec is only for C++.
 */
export interface Spec extends TurboModule {
	App: (
		/**
		 * @see {@link file:///./../../types/AppOptions.ts}
		 */
		appOptions?: CodegenTypes.UnsafeObject,
	) => CodegenTypes.UnsafeObject,

	SSLApp: (
		/**
		 * @see {@link file:///./../../types/AppOptions.ts}
		 */
		appOptions: CodegenTypes.UnsafeObject,
	) => CodegenTypes.UnsafeObject,

	getParts: (
		body: CodegenTypes.UnsafeObject,
		contentType: CodegenTypes.UnsafeObject,
	) => void,

	us_listen_socket_close: (
		listenSocket: CodegenTypes.UnsafeObject,
	) => void,

	us_socket_local_port: (
		socket: CodegenTypes.UnsafeObject,
	) => void,
}

export default TurboModuleRegistry.getEnforcing<Spec>(
	"ReactNativeUws",
)
