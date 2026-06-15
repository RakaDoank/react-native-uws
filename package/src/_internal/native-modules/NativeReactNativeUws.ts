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

	getParts: (
		/**
		 * Has to be an ArrayBuffer.
		 * Codegen map this to facebook::jsi::Object.
		 */
		body: unknown,
		contentType: string,
	) => unknown, // Codegen map this to facebook::jsi::Object

	_us_listen_socket_close: (
		id: CodegenTypes.Int32,
		// listenSocket: CodegenTypes.UnsafeObject,
	) => void,

	_us_socket_local_port: (
		id: CodegenTypes.Int32,
		// socket: CodegenTypes.UnsafeObject,
	) => number,
}

export default TurboModuleRegistry.getEnforcing<Spec>(
	"ReactNativeUws",
)
