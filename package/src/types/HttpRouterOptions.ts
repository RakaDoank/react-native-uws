export interface HttpRouterOptions {
	/**
	 * Optimize specific route handler by disable body reading.
	 * The `onData`, `onDataV2`, and `onFullData` method from response object will do nothing
	 * if the body read is disabled.
	 * 
	 * This is an actual optimization, because react-native-uws has to predefined the body read
	 * (the res->onDataV2) internally from C++ side by default. So if you disable body reading
	 * in a specific route handler, react-native-uws will not do the predefined res->onDataV2 from C++ side at all.
	 * 
	 * The reason why react-native-uws has to predefined the res->onDataV2 from C++ side, it is because we cannot do a sync call from uWebSockets C++ which run in arbitrary thread to JS, and late onDataV2 assignment will losing the body data. It is also by design that we run uWebSockets in another thread to prevent Main/UI thread blocking.
	 */
	disableBodyRead?: boolean,

	/**
	 * Limit max body data size reading in byte.
	 * 
	 * This is useful to help the method from `HttpResponse` object
	 * like `onData`, `onDataV2`, and `onFullData` to limit the chunk you need to read from a request.
	 * 
	 * This option does nothing if `disableBodyRead` option is true.
	 */
	maxBodySize?: number,
}
