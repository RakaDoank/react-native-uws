# uws-react-native

The port of [**uWebSockets**](https://github.com/uNetworking/uWebSockets) library for React Native. Inspired by the [**uWebSockets.js**](https://github.com/uNetworking/uWebSockets.js) for Node.js and tailored for React Native app.

- **uWebSockets** is simple, secure & standards compliant web server for the most demanding of applications. See [**uWebSockets**](https://github.com/uNetworking/uWebSockets) for more information
- **uws-react-native** is library that allows you to easily use **uWebSockets** in your React Native through new architecture with JSI to the native C++

> ⚠️ uws-react-native is still in heavy development and research. All the uWebSockets instances are not fully implemented yet.

## Installation

We provide two registry homes where this library lives which can be used for your project. You can use either it's from NPM registry or GitHub Packages

### From NPM Registry

npm
```
npm install uws-react-native
```

yarn
```
yarn add uws-react-native
```

pnpm
```
pnpm install uws-react-native
```

### From GitHub Packages

First, create your GitHub personal token (classic) with `read:packages` access
> **Skip if you already have it**
1. In the upper-right corner of any page on GitHub, click your profile picture, then click Settings
2. In the left sidebar, click **<> Developer settings**
3. In the left sidebar, under **🔑 Personal access tokens**, click **Tokens (classic)**
4. Select Generate new token, then click **Generate new token (classic)**
5. Fill your descriptive note, fill / check the `read:packages` access, and click **Generate token**
6. Copy the generated token, or save it to somewhere else

After that, in the same directory as your `package.json` file, create or edit an `.npmrc` file with this snippet
```
@rakadoank:registry=https://npm.pkg.github.com
//npm.pkg.github.com/:_authToken=YOUR_GITHUB_PERSONAL_TOKEN
```

Finally, do the regular installation with the CLI, but with the scope name

npm
```
npm install @rakadoank/uws-react-native
```

yarn
```
yarn add @rakadoank/uws-react-native
```

pnpm
```
pnpm install @rakadoank/uws-react-native
```


## Usage

Since uws-react-native is a port of uWebSockets library for JavaScript, this library is really similar of how to use the [uWebSockets](https://github.com/uNetworking/uWebSockets) in C++, or the [uWebSockets.js](https://github.com/uNetworking/uWebSockets.js) for Node.js.
You can refer to the [uWebSockets.js](https://github.com/uNetworking/uWebSockets.js) example.

This repository does also contain the [example](https://github.com/RakaDoank/uws-react-native/blob/main/example/src/app/index.tsx) app. You can also refer to it, but it is currently for library development playground rather than proper example.

## Compatibility

`uws-react-native` is only compatible for new architecture of React Native.

### Platform

uws-react-native is supported in Android, iOS, and macOS. Windows is currently not supported, and Web is impossible.

### Unsupported APIs

- **SSL options**. We need to research how to provide .pem file in React Native app and its platform architecture regarding the SSL topic
- **WebSocket server**. Not implemented yet. It will be supported later.

### New APIs

There are some new APIs in uws-react-native. The most reason is because of the differentiation of React Native's architecture compared to the casual server program

- The route method handler like `any`, `get`, `post` and others expects third argument as the handler options optionally. See [`HttpRouterOptions`](https://github.com/RakaDoank/uws-react-native/blob/main/package/src/types/HttpRouterOptions.ts)
- In [`HttpResponse`](https://github.com/RakaDoank/uws-react-native/blob/main/package/src/types/HttpResponse.ts) object,
  - `onFullData`. This is an equivalent of `res.collectBody` from uWebSockets.js, but to set the max size, provide it through route method handler options
  - `onFullDataText`. Same as the `onFullData`, but provides the body data in raw text instead of ArrayBuffer. This is useful if you are using React Native 0.84 version or older because TextDecoder is natively supported only in React Native 0.85 version

---

## Development & Research

uws-react-native is still in heavy development and research. All the uWebSockets instances are not fully implemented yet.

There are development and research things that you can read for your informations, especially if you are already familiar with uWebSockets. The most things are because of the React Native's architecture differentiation compared to the casual uWebSockets server program.

### Threading

We are embracing the main chaos of supporting uWebSockets in React Native architecture, which is to prevent UI/Main thread from blocking.

Intentionally, we make the uWebSockets will runs in another thread, therefore we have to make sure the communication safety between uWebSockets runner thread to the JS thread and vice versa. In theory, we can make uWebSockets runs in the main thread, but the app will be unusable, and then force closing the app is the only way to stop the server.

Yet, we only use one dedicated thread per uWebSockets run. I am in my own research to improve this by maximizing it to the hardware concurrency limit, but also to be careful about JavaScript runtime, because we have no control of it.

We have another issue because of the uWebSockets runs in another thread. From the JSI C++ side, we have to assume any JavaScript functions as a callback especially the route method handler are asynchronous. We cannot make a sync call to the JavaScript function from an arbitrary thread to the JavaScript thread, and due to the asynchronous call, it makes the JavaScript call to the uWebSockets instances is late.

There some topics you may to read regarding this threading research

#### Predefined Instances

For the threading case especially, we have to predefined two instances in the C++ uWebSockets route method handler because of the late call, and the late call would not be tolerated by the uWebSockets internally for some cases

- `res.onAborted`

We have to predefined the `onAborted` callback in C++ side and prevent `res.end` call from JavaScript internally in C++ side. Normally, you would check if it is already aborted or not before calling the end method, but our predefined `onAborted` callback may also late to tell to the JavaScript side. If we are not predefining this method, uWebSockets assumes that you have not assign `onAborted` callback at the time due to the `res.onAborted` assignment from JavaScript is also late.

- `res.onDataV2`

For the `onData` and `onDataV2`, we have predefined it with a single `onDataV2` callback in C++ side. If this method is not predefined, uWebSockets will not collect any incoming body data. If a route method handler is sure that it doesn't expect any body data at all, a route method handler can disable the body reading through the third argument in `any`, `get`, `post`, and other route methods.

#### About Worker Thread

In theory, we can create another JavaScript runtime with [react-native-worklets](https://docs.swmansion.com/react-native-worklets/) and tie with our uWebSockets runner. It would solve a lot of late communication problem between uWebSockets runner and JavaScript thread that we embrace right now, but it also introduces new major issue, which is developer experience. It sounds like not a big problem, but it may bigger than you think.

Think of this sample case, you want to use uws-react-native server for a simple CRUD with a local database in an app. You probably know SQLite can be used in Android, iOS, macOS, even Windows app, with your own adapter or a known library that support SQLite integration in React Native such as [op-sqlite](https://github.com/OP-Engineering/op-sqlite), and [Expo SQLite](https://docs.expo.dev/versions/latest/sdk/sqlite). If you want to use that known library, this case would not works at all, because op-sqlite and/or Expo SQLite is tied to the default JavaScript runtime. Even, you cannot use any React Native non-JS-only libraries in arbitrary JavaScript thread that has created by react-native-worklets if the library you want to use is using the default JavaScript runtime. If you really want to achieve the goal of this case, you have to create your own library for the SQLite database by yourself and tie-up with the JavaScript runtime. Probably, in the future we would still provide that worker thread with react-native-worklets.
