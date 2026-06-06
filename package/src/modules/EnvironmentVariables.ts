/**
 * Environment variables recognized by uWebSockets.js.
 *
 * These can be set in the process environment before starting the server.
 */
export namespace EnvironmentVariables {
	/**
     * Maximum total byte size of HTTP request headers. This is a runtime env variable. Default: 4096.
     */
	export const UWS_HTTP_MAX_HEADERS_SIZE: string | undefined = "TODO"

	/**
     * Maximum number of HTTP request headers. This is a compile-time define, not a runtime env variable. Default: 100.
     */
	export const UWS_HTTP_MAX_HEADERS_COUNT: string | undefined = "TODO"
}
