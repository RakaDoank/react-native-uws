import NativeReactNativeUws from "../_internal/native-modules/NativeReactNativeUws"

import type {
	AppOptions,
	TemplatedApp,
} from "../types"

/**
 * Constructs an SSL app. See `App`.
 */
export function SSLApp(options: AppOptions) : TemplatedApp {
	return NativeReactNativeUws.SSLApp(options) as unknown as TemplatedApp
}
