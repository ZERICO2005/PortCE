/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef ENGINE_H
#define ENGINE_H

#include "../PortCE_Common.h"

/* Enum of Functions that can be activated by a KeyboardScancode */
namespace Key_Function {

	const char* const Key_Function_Text[] = {
		"NONE",
		/* Cordinates */
			"incRealPos", "decRealPos", "resetRealPos",
			"incImagPos", "decImagPos", "resetImagPos",
			"incZoom"   , "decZoom"   , "resetZoom"   ,
			"resetCoordinates",
			"COORDINATES",
		/* Transformations */
			"clockwiseRot"     , "counterclockwiseRot"    , "resetRotation",
			"clockwiseRot90"   , "counterclockwiseRot90"  , "rotate180"    ,
			"clockwiseRot15"   , "counterclockwiseRot15"  ,
			"clockwiseRot5"    , "counterclockwiseRot5"   ,
			// "clockwiseRotStep" , "counterclockwiseRotStep",
			"clockwiseRotPower", "counterclockwiseRotPower",
			// "flipHorizontal", "flipVertical", "resetFlip",
			"incStretch", "decStretch", "resetStretch", "reverseStretch",
			"resetTransformations",
			"TRANSFORMATIONS",
		/* Julia */
			"toggleJulia",
			"toggleStartingZ",
			"toggleCursorZValue",
			"toggleRelativeZValue",
			"incZReal", "decZReal", "resetZReal",
			"incZImag", "decZImag", "resetZImag",
			"JULIA",
		/* Parameters */
			"incMaxItr", "decMaxItr", "resetMaxItr",
			// "toggleAutoItr",
			"incBreakout", "decBreakout", "resetBreakout",
			"toggleAdjustZoomToPower",
			"PARAMETERS",
		/* Polar */
			"togglePolarMandelbrot",
			"roundPower", "floorPower", "ceilingPower",
			"toggleIntegerPower",
			"lockToCardioid",
			"flipCardioidSide",
			"POLAR",
		/* Formula */
			"inputFormula", "resetFormula",
			"incFormula"  , "decFormula"  ,
			"incFamily"   , "decFamily"   ,
			"inputPower"  , "resetPower"  ,
			"incPower"    , "decPower"    ,
			"FORMULA",
		/* Screen Split */
			// "toggleScreenSplit", "nextScreenSplit", "prevScreenSplit",
			// "clockwiseCornerSplit", "counterClockwiseCornerSplit",
			// "toggleFloatingWindow",
			// "swapPrimaryAndSecondary",
			// "SCREEN_SPLIT",
		/* Functions */
			"closeMenu",
			"openCordinateMenu",
			"openFractalMenu",
			// "exportFractal",
			// "importFractal",
			"takeScreenshot",
			"takeSuperScreenshot",
			"openRenderingMenu",
			"openSettingsMenu",
			"openKeyBindsMenu",
			"abortRendering",
			"exitApplication",
			"toggleFullscreen",
			// "toggleMenuBar", "toggleCordinateBar", "toggleButtonBar",
			"FUNCTIONS",
		/* Rendering */
			"refreshFractal",
			// "switchCPUandGPU",
			/* "fp16GpuRendering", */ "fp32GpuRendering", "fp64GpuRendering",
			/* "fp32x2fp32GpuRendering", "fp64x2fp32GpuRendering", */
			"fp32CpuRendering", "fp64CpuRendering", "fp80CpuRendering", "fp128CpuRendering",
			"fp32x2CpuRendering", "fp64x2CpuRendering", "fp80x2CpuRendering",
			"incSuperSample"  , "decSuperSample"  , "resetSuperSample",
			"incSubSample"    , "decSubSample"    , "resetSubSample"  ,
			"RENDERING"
	};

	enum Key_Function_Enum {
		NONE,
		/* Cordinates */
			incRealPos, decRealPos, resetRealPos,
			incImagPos, decImagPos, resetImagPos,
			incZoom   , decZoom   , resetZoom   ,
			resetCoordinates,
			COORDINATES,
		/* Transformations */
			clockwiseRot     , counterclockwiseRot    , resetRotation,
			clockwiseRot90   , counterclockwiseRot90  , rotate180    ,
			clockwiseRot15   , counterclockwiseRot15  ,
			clockwiseRot5    , counterclockwiseRot5   ,
			// clockwiseRotStep , counterclockwiseRotStep,
			clockwiseRotPower, counterclockwiseRotPower,
			// flipHorizontal, flipVertical, resetFlip,
			incStretch, decStretch, resetStretch, reverseStretch,
			resetTransformations,
			TRANSFORMATIONS,
		/* Julia */
			toggleJulia,
			toggleStartingZ,
			toggleCursorZValue,
			toggleRelativeZValue,
			incZReal, decZReal, resetZReal,
			incZImag, decZImag, resetZImag,
			JULIA,
		/* Parameters */
			incMaxItr, decMaxItr, resetMaxItr,
			// toggleAutoItr,
			incBreakout, decBreakout, resetBreakout,
			toggleAdjustZoomToPower,
			PARAMETERS,
		/* Polar */
			togglePolarMandelbrot,
			roundPower, floorPower, ceilingPower,
			toggleIntegerPower,
			lockToCardioid,
			flipCardioidSide,
			POLAR,
		/* Formula */
			inputFormula, resetFormula,
			incFormula  , decFormula  ,
			incFamily   , decFamily   ,
			inputPower  , resetPower  ,
			incPower    , decPower    ,
			FORMULA,
		/* Screen Split */
			// toggleScreenSplit, nextScreenSplit, prevScreenSplit,
			// clockwiseCornerSplit, counterClockwiseCornerSplit,
			// toggleFloatingWindow,
			// swapPrimaryAndSecondary,
			// SCREEN_SPLIT,
		/* Functions */
			closeMenu,
			openCordinateMenu,
			openFractalMenu,
			// exportFractal,
			// importFractal,
			takeScreenshot,
			takeSuperScreenshot,
			openRenderingMenu,
			openSettingsMenu,
			openKeyBindsMenu,
			abortRendering,
			exitApplication,
			toggleFullscreen,
			// toggleMenuBar, toggleCordinateBar, toggleButtonBar,
			FUNCTIONS,
		/* Rendering */
			refreshFractal,
			// switchCPUandGPU,
			/* fp16GpuRendering, */ fp32GpuRendering, fp64GpuRendering,
			/* fp32x2fp32GpuRendering, fp64x2fp32GpuRendering, */
			fp32CpuRendering, fp64CpuRendering, fp80CpuRendering, fp128CpuRendering,
			fp32x2CpuRendering, fp64x2CpuRendering, fp80x2CpuRendering,
			incSuperSample  , decSuperSample  , resetSuperSample,
			incSubSample    , decSubSample    , resetSubSample  ,
			RENDERING,
		Parameter_Function_Count
	};
	static_assert(
		Parameter_Function_Count == ARRAY_LENGTH(Key_Function_Text),
		"Parameter_Function_Count != ARRAY_LENGTH(Key_Function_Text)"
	);

	const size_t Key_Function_Map[] = {
		NONE,
		COORDINATES,
		TRANSFORMATIONS,
		JULIA,
		PARAMETERS,
		POLAR,
		FORMULA,
		// SCREEN_SPLIT,
		FUNCTIONS,
		RENDERING
	};
	const char* const Key_Function_Category_Text[] = {
		"NONE",
		"COORDINATES",
		"TRANSFORMATIONS",
		"JULIA",
		"PARAMETERS",
		"POLAR",
		"FORMULA",
		// "SCREEN_SPLIT",
		"FUNCTIONS",
		"RENDERING"
	};
	static_assert(
		ARRAY_LENGTH(Key_Function_Map) == ARRAY_LENGTH(Key_Function_Category_Text),
		"ARRAY_LENGTH(Key_Function_Map) != ARRAY_LENGTH(Key_Function_Category_Text)"
	);

	enum Key_Function_Category {
		Category_NONE,
		Category_COORDINATES,
		Category_TRANSFORMATIONS,
		Category_JULIA,
		Category_PARAMETERS,
		Category_POLAR,
		Category_FORMULA,
		// Category_SCREEN_SPLIT,
		Category_FUNCTIONS,
		Category_RENDERING
	};
};
/* String of Functions that can be activated by a KeyboardScancode */
//const char*

#endif /* ENGINE_H */
