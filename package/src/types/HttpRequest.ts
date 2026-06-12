import type {
	RecognizedString,
} from "./RecognizedString"

/**
 * An HttpRequest is stack allocated and only accessible during the callback invocation.
 */
export interface HttpRequest {
	/**
	 * Loops over all headers.
	 */
	forEach(
		cb: (key: string, value: string) => void,
	) : void,

	/**
	 * Returns the HTTP method as-is.
	 */
	getCaseSensitiveMethod() : string,

	/**
	 * Returns the lowercased header value or empty string.
	 */
	getHeader(
		lowerCaseKey: RecognizedString,
	) : string,

	/**
	 * Returns the lowercased HTTP method, useful for "any" routes.
	 */
	getMethod() : string,

	/**
	 * Returns the parsed parameter at index. Corresponds to route. Can also take the name of the parameter.
	 * 
	 * ---
	 * 
	 * **Note**
	 * 
	 * For this moment, find by an index number is much faster.
	 * 
	 * This is because we have to reimplement this method due to `req->getParameter()` problem internally in C++ that always returned `nullptr` data of `std::string_view` if we does it in JSI object.
	 * 
	 * @example
	 * ```js
	 * app.get("/profile/:userID/:type", (res, req) => {
	 *   // get userID
	 *   console.log("userID", req.getParameter(0), req.getParameter("userID"))
	 * 
	 *   // get type
	 *   console.log("type", req.getParameter(1), req.getParameter("type"))
	 * })
	 * ```
	 */
	getParameter(
		index: number | RecognizedString,
	) : string | undefined,

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
	 * Returns the URL including initial /slash
	 */
	getUrl() : string,

	/**
	 * Setting yield to true is to say that this route handler did not handle the route, causing the router to continue looking for a matching route handler, or fail.
	 */
	setYield(
		_yield: boolean,
	) : HttpRequest,
}
