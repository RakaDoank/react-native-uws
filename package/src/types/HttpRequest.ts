import type {
	RecognizedString,
} from "./RecognizedString"

/**
 * An HttpRequest is stack allocated and only accessible during the callback invocation.
 */
export interface HttpRequest {
	/**
	 * Returns the lowercased header value or empty string.
	 */
	getHeader(
		lowerCaseKey: RecognizedString,
	) : string,

	/**
	 * Returns the parsed parameter at index. Corresponds to route. Can also take the name of the parameter.
	 */
	getParameter(
		index: number | RecognizedString,
	) : string | undefined,

	/**
	 * Returns the URL including initial /slash
	 */
	getUrl() : string,

	/**
	 * Returns the lowercased HTTP method, useful for "any" routes.
	 */
	getMethod() : string,

	/**
	 * Returns the HTTP method as-is.
	 */
	getCaseSensitiveMethod() : string,

	/**
	 * Returns the raw querystring (the part of URL after ? sign) or empty string.
	 */
	getQuery() : string,

	/**
	 * Returns a decoded query parameter value or undefined.
	 */
	getQuery(
		key: string,
	) : string | undefined,

	/**
	 * Loops over all headers.
	 */
	forEach(
		cb: (key: string, value: string) => void,
	) : void,

	/**
	 * Setting yield to true is to say that this route handler did not handle the route, causing the router to continue looking for a matching route handler, or fail.
	 */
	setYield(
		_yield: boolean,
	) : HttpRequest,
}
