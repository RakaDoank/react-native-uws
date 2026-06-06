# react-native-uws

A library to embed a HTTP server in a React Native app with thread safety to handle high-volume and high-pressure requests.

> ⚠️ The library is still in development and research. I'm still playing around with this library. Read [New Research](#new-research)

See this [example](https://github.com/RakaDoank/react-native-uws/blob/main/example/src/app/index.tsx) of how to use this library.

## Features

- Built-in support for `JSON` thanks to the nature of JavaScript
- Memory optimized for `multipart/form-data` request handling support with its cached file instead of holding an array buffer

#### Pending
- Static file and/or directory serving
- WebSocket support for real time bidirectional communication

## New Research
I'm still in development and research to make this library performs better in high-volume and high-pressure requests. Currently, migrating to [uWebSockets](https://github.com/uNetworking/uWebSockets) server to support cross platforms and better performance since React Native new architecture can make JavaScript side communicate back and forth with C++ through JSI with a little overhead.

## Old Research
See [react-native-echo](https://github.com/RakaDoank/react-native-echo).