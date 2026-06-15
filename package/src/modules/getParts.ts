import NativeReactNativeUws from "../_internal/native-modules/NativeReactNativeUws"

import type {
	MultipartField,
} from "../types"

/**
 * Takes a POSTed body and contentType, and returns an array of parts if the request is a multipart request
 */
export function getParts(
	body: ArrayBuffer,
	contentType: string,
) : MultipartField[] | undefined {
	const result = NativeReactNativeUws.getParts(body, contentType)

	// See /react-native-uws/package/cpp/ReactNativeUwsModule.cpp
	// Codegen maps the `unknown` type to jsi::Object
	// We can't return Array | undefined in that getParts function.
	// So we have to remap it.

	// If the result is empty object, it's undefined
	// Otherwise, it's a valid MultipartField array

	if(Array.isArray(result)) {
		return result as MultipartField[]
	}

	return undefined
}
