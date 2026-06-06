import NativeReactNativeEcho from "../_internal/native-modules/NativeReactNativeUws"

/**
 * Takes a POSTed body and contentType, and returns an array of parts if the request is a multipart request
 */
export function getParts(
	body: RecognizedString,
	contentType: RecognizedString,
) : MultipartField[] | undefined {
	// TODO
}
