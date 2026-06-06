import {
	View,
	type ViewProps,
} from "react-native"

import {
	Stack,
} from "expo-router"

import {
	CarbonReactNative,
	CarbonStyleSheet,
} from "@audira/carbon-react-native"

export default function Layout() {

	return (
		<CarbonReactNative>
			<BgView>
				<Stack
					screenOptions={{
						headerShown: false,
					}}
				/>
			</BgView>
		</CarbonReactNative>
	)

}

const
	carbonStyleSheet =
		CarbonStyleSheet.create({
			bgView: {
				backgroundColor: CarbonStyleSheet.color.background,
			},
		})

interface BgViewProps extends ViewProps {
}
function BgView({
	style,
	...props
}: BgViewProps) {

	CarbonStyleSheet.use()

	return (
		<View
			{ ...props }
			style={ [
				CarbonStyleSheet.g.flex_auto,
				carbonStyleSheet.bgView,
				style,
			] }
		/>
	)

}
