/**
 * Options used when constructing an app. Especially for SSLApp.
 * These are options passed directly to uSockets, C layer.
 */
export interface AppOptions {
	key_file_name?: string,

	cert_file_name?: string,

	ca_file_name?: string,

	passphrase?: string,

	dh_params_file_name?: string,

	ssl_ciphers?: string,

	/**
	 * This translates to SSL_MODE_RELEASE_BUFFERS
	 */
	ssl_prefer_low_memory_usage?: boolean,
}
