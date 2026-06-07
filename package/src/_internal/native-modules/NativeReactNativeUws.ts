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

	// /**
	//  * For get, post, put, del, options, head, connect, trace, and any.
	//  */
	// App_router: (
	// 	appID: string,
	// 	pattern: string,
	// 	callback: (
	// 		requestObject: CodegenTypes.UnsafeObject,
	// 		responseObject: CodegenTypes.UnsafeObject,
	// 		dispatcher: (
	// 			responseObject: CodegenTypes.UnsafeObject,
	// 			requestObject: CodegenTypes.UnsafeObject,
	// 		) => void,
	// 	) => void,
	// ) => void,

	// App_listen: (
	// 	appID: string,
	// 	params: CodegenTypes.UnsafeObject,
	// ) => void,

	// SSLApp: (
	// 	/**
	// 	 * @see {@link file:///./../../types/AppOptions.ts}
	// 	 */
	// 	appOptions: CodegenTypes.UnsafeObject,
	// ) => CodegenTypes.UnsafeObject,

	getParts: (
		body: CodegenTypes.UnsafeObject,
		contentType: CodegenTypes.UnsafeObject,
	) => CodegenTypes.UnsafeObject,

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
