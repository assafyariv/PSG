/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Avocado.h"
#include "AvocadoDoc.h"
#include "AvocadoView.h"
#include "MainFrm.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_COMMAND(ID_ADDSECTION_PLANE, &CMainFrame::OnAddSectionPlane)
	ON_UPDATE_COMMAND_UI(ID_ADDSECTION_PLANE, &CMainFrame::OnUpdateAddSectionPlane)
	ON_UPDATE_COMMAND_UI(ID_SELECT_PART, &CMainFrame::OnUpdateSelectPart)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_FIT_PAGE, &CMainFrame::OnUpdateOrientationFitPage)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_BACK, &CMainFrame::OnUpdateOrientationBack)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_FRONT, &CMainFrame::OnUpdateOrientationFront)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_LEFT, &CMainFrame::OnUpdateOrientationLeft)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_RIGHT, &CMainFrame::OnUpdateOrientationRight)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_TOP, &CMainFrame::OnUpdateOrientationTop)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_BOTTOM, &CMainFrame::OnUpdateOrientationBottom)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_CUSTOM, &CMainFrame::OnUpdateOrientationCustom)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_DEF1, &CMainFrame::OnUpdateOrientationDef1)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_DEF2, &CMainFrame::OnUpdateOrientationDef2)
	ON_UPDATE_COMMAND_UI(ID_SELECT_AREA, &CMainFrame::OnUpdateSelectArea)
	ON_UPDATE_COMMAND_UI(ID_SELECT_ALL, &CMainFrame::OnUpdateSelectAll)
	ON_UPDATE_COMMAND_UI(ID_STYLE_FLAT, &CMainFrame::OnUpdateStyleFlat)
	ON_UPDATE_COMMAND_UI(ID_STYLE_HLR, &CMainFrame::OnUpdateStyleHlr)
	ON_UPDATE_COMMAND_UI(ID_STYLE_TOON, &CMainFrame::OnUpdateStyleToon)
	ON_UPDATE_COMMAND_UI(ID_STYLE_SHADED, &CMainFrame::OnUpdateStyleShaded)
	ON_UPDATE_COMMAND_UI(ID_STYLE_ORTHOGRAPHIC_VIEW, &CMainFrame::OnUpdateStyleOrthographicView)
	ON_COMMAND(ID_STYLE_SHADED, &CMainFrame::OnStyleShaded)
	ON_COMMAND(ID_STYLE_TOON, &CMainFrame::OnStyleToon)
	ON_COMMAND(ID_STYLE_HLR, &CMainFrame::OnStyleHlr)
	ON_COMMAND(ID_STYLE_FLAT, &CMainFrame::OnStyleFlat)
	ON_UPDATE_COMMAND_UI(ID_MENUS_STYLE, &CMainFrame::OnUpdateMenusStyle)
	ON_UPDATE_COMMAND_UI(ID_MENUS_SELECT, &CMainFrame::OnUpdateMenusSelect)
	ON_UPDATE_COMMAND_UI(ID_MENUS_ORIENTAION, &CMainFrame::OnUpdateMenusOrientaion)
	ON_COMMAND(ID_STYLE_ORTHOGRAPHIC_VIEW, &CMainFrame::OnStyleOrthographicView)
	ON_UPDATE_COMMAND_UI(ID_APPREANCE_HIDE, &CMainFrame::OnUpdateAppreanceHide)
//	ON_UPDATE_COMMAND_UI(ID_AVOCADOVIEW_MATERIAL, &CMainFrame::OnUpdateAvocadoviewMaterial)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, &CMainFrame::OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CMainFrame::OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CMainFrame::OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_ORIENTATION_LOOK_AT, &CMainFrame::OnUpdateOrientationLookAt)
	ON_COMMAND(ID_EDIT_COPY, &CMainFrame::OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, &CMainFrame::OnEditClear)
	ON_COMMAND(ID_EDIT_CUT, &CMainFrame::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CMainFrame::OnUpdateEditPaste)
	ON_COMMAND(ID_MODEL_MOVE, &CMainFrame::OnModelMove)
	ON_UPDATE_COMMAND_UI(ID_MODEL_MOVE, &CMainFrame::OnUpdateModelMove)
	ON_COMMAND(ID_MODEL_ROTATE, &CMainFrame::OnModelRotate)
	ON_UPDATE_COMMAND_UI(ID_MODEL_ROTATE, &CMainFrame::OnUpdateModelRotate)
	ON_COMMAND(ID_MODEL_SCALE, &CMainFrame::OnModelScale)
	ON_UPDATE_COMMAND_UI(ID_MODEL_SCALE, &CMainFrame::OnUpdateModelScale)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_ANNOTATION, &CMainFrame::OnUpdateDisplayAnnotation)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_BACKGROUND, &CMainFrame::OnUpdateDisplayBackground)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_CAMERAS, &CMainFrame::OnUpdateDisplayCameras)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_LIGHTS, &CMainFrame::OnUpdateDisplayLights)
	ON_UPDATE_COMMAND_UI(ID_DISPLAY_SPINCENTER, &CMainFrame::OnUpdateDisplaySpincenter)
	ON_COMMAND(ID_DISPLAY_SPINCENTER, &CMainFrame::OnDisplaySpincenter)
	ON_COMMAND(ID_DISPLAY_LIGHTS, &CMainFrame::OnDisplayLights)
	ON_COMMAND(ID_DISPLAY_CAMERAS, &CMainFrame::OnDisplayCameras)
	ON_COMMAND(ID_DISPLAY_ANNOTATION, &CMainFrame::OnDisplayAnnotation)
	ON_COMMAND(ID_DISPLAY_BACKGROUND, &CMainFrame::OnDisplayBackground)
	ON_UPDATE_COMMAND_UI(ID_TRACKING_USERMODE, &CMainFrame::OnUpdateTrackingUsermode)
	ON_UPDATE_COMMAND_UI(ID_TRACKING_FLYMODE, &CMainFrame::OnUpdateTrackingFlymode)
	ON_UPDATE_COMMAND_UI(ID_TRACKING_CADMODE, &CMainFrame::OnUpdateTrackingCadmode)
	ON_COMMAND(ID_TRACKING_CADMODE, &CMainFrame::OnTrackingCadmode)
	ON_COMMAND(ID_TRACKING_FLYMODE, &CMainFrame::OnTrackingFlymode)
	ON_COMMAND(ID_TRACKING_USERMODE, &CMainFrame::OnTrackingUsermode)
	ON_COMMAND(ID_GRAPHICS_RAYTRACING, &CMainFrame::OnGraphicsRaytracing)
	ON_UPDATE_COMMAND_UI(ID_GRAPHICS_RAYTRACING, &CMainFrame::OnUpdateGraphicsRaytracing)
	ON_COMMAND(ID_GRAPHICS_ANTI_ALIASING, &CMainFrame::OnGraphicsAntiAliasing)
	ON_UPDATE_COMMAND_UI(ID_GRAPHICS_ANTI_ALIASING, &CMainFrame::OnUpdateGraphicsAntiAliasing)
	ON_UPDATE_COMMAND_UI(ID_GRAPHICS_STEREO, &CMainFrame::OnUpdateGraphicsStereo)
	ON_COMMAND(ID_GRAPHICS_STEREO, &CMainFrame::OnGraphicsStereo)
	ON_UPDATE_COMMAND_UI(ID_APPERANCE_UNHIDE, &CMainFrame::OnUpdateApperanceUnhide)
	ON_COMMAND(ID_APPREANCE_HIDE, &CMainFrame::OnAppreanceHide)
	ON_COMMAND(ID_APPREANCE_UNHIDEALL, &CMainFrame::OnAppreanceUnhideall)
	ON_COMMAND(ID_APPERANCE_UNHIDE, &CMainFrame::OnApperanceUnhide)
ON_UPDATE_COMMAND_UI(ID_APPREANCE_UNHIDEALL, &CMainFrame::OnUpdateAppreanceUnhideall)
ON_COMMAND(ID_ORIENTATION_BACK, &CMainFrame::OnOrientationBack)
ON_COMMAND(ID_ORIENTATION_BOTTOM, &CMainFrame::OnOrientationBottom)
ON_COMMAND(ID_ORIENTATION_CUSTOM, &CMainFrame::OnOrientationCustom)
ON_COMMAND(ID_ORIENTATION_DEF1, &CMainFrame::OnOrientationDef1)
ON_COMMAND(ID_ORIENTATION_DEF2, &CMainFrame::OnOrientationDef2)
ON_COMMAND(ID_ORIENTATION_FRONT, &CMainFrame::OnOrientationFront)
ON_COMMAND(ID_ORIENTATION_TOP, &CMainFrame::OnOrientationTop)
ON_COMMAND(ID_ORIENTATION_LEFT, &CMainFrame::OnOrientationLeft)
ON_COMMAND(ID_ORIENTATION_RIGHT, &CMainFrame::OnOrientationRight)
ON_COMMAND(ID_ORIENTATION_LOOK_AT, &CMainFrame::OnOrientationLookAt)
ON_UPDATE_COMMAND_UI(ID_AVOCADOVIEW_MATERIAL, &CMainFrame::OnUpdateAvocadoviewMaterial)
ON_COMMAND(ID_EDIT_PASTE, &CMainFrame::OnEditPaste)
ON_COMMAND(ID_SPLIT_ELEMENT, &CMainFrame::OnSplitElement)
ON_UPDATE_COMMAND_UI(ID_SPLIT_ELEMENT, &CMainFrame::OnUpdateSplitElement)
ON_COMMAND(ID_TRACKING_PRESELECTIONHIGHTLIGHTS, &CMainFrame::OnTrackingPreselectionhightlights)
ON_UPDATE_COMMAND_UI(ID_TRACKING_PRESELECTIONHIGHTLIGHTS, &CMainFrame::OnUpdateTrackingPreselectionhightlights)
ON_COMMAND(ID_MATERIAL_COLOR, &CMainFrame::OnMaterialColor)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_COLOR, &CMainFrame::OnUpdateMaterialColor)
ON_COMMAND(ID_APPERANCE_MATERIAL0, &CMainFrame::OnApperanceMaterial0)
ON_COMMAND(ID_APPERANCE_MATERIAL2, &CMainFrame::OnApperanceMaterial2)
ON_COMMAND(ID_APPERANCE_MATERIAL3, &CMainFrame::OnApperanceMaterial3)
ON_COMMAND(ID_APPERANCE_MATERIAL4, &CMainFrame::OnApperanceMaterial4)
ON_COMMAND(ID_APPERANCE_MATERIAL5, &CMainFrame::OnApperanceMaterial5)
ON_COMMAND(ID_APPERANCE_MATERIAL6, &CMainFrame::OnApperanceMaterial6)
ON_COMMAND(ID_APPERANCE_MATERIAL7, &CMainFrame::OnApperanceMaterial7)
ON_COMMAND(ID_APPERANCE_MATERIAL1, &CMainFrame::OnApperanceMaterial1)
ON_COMMAND(ID_ORIENTATION_FIT_PAGE, &CMainFrame::OnOrientationFitPage)
ON_COMMAND(ID_APPERANCE_MATERIAL_8, &CMainFrame::OnApperanceMaterial8)
ON_COMMAND(ID_APPERANCE_MATERIAL_9, &CMainFrame::OnApperanceMaterial9)
ON_COMMAND(ID_MODEL_RESETTODEFAULT, &CMainFrame::OnModelResettodefault)
ON_COMMAND(ID_MODEL_RESETTOORI, &CMainFrame::OnModelResettoori)
ON_UPDATE_COMMAND_UI(ID_MODEL_RESET, &CMainFrame::OnUpdateModelReset)
ON_COMMAND(ID_LAYOUT_NEWPAGE, &CMainFrame::OnLayoutNewpage)
ON_COMMAND(ID_LAYOUT_NEXTPAGE, &CMainFrame::OnLayoutNextpage)
ON_COMMAND(ID_LAYOUT_BACKPAGE, &CMainFrame::OnLayoutBackpage)
ON_COMMAND(ID_LAYOUT_DELETEPAGE, &CMainFrame::OnLayoutDeletepage)
ON_COMMAND(ID_LAYOUT_DUPLICATEPAGE, &CMainFrame::OnLayoutDuplicatepage)
ON_COMMAND(ID_APPERANCE_MATERIAL14, &CMainFrame::OnApperanceMaterial14)
ON_COMMAND(ID_APPERANCE_MATERIAL13, &CMainFrame::OnApperanceMaterial13)
ON_COMMAND(ID_APPERANCE_MATERIAL15, &CMainFrame::OnApperanceMaterial15)
ON_COMMAND(ID_APPERANCE_MATERIAL10, &CMainFrame::OnApperanceMaterial10)
ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE2, &CMainFrame::OnUpdateIndicator)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_PLASTIC, &CMainFrame::OnUpdateMaterialPlastic)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_WOOD, &CMainFrame::OnUpdateMaterialWood)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_STONE, &CMainFrame::OnUpdateMaterialStone)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_GLASS, &CMainFrame::OnUpdateMaterialGlass)
ON_UPDATE_COMMAND_UI(ID_MATERIAL_FABRIC, &CMainFrame::OnUpdateMaterialFabric)
ON_COMMAND(ID_AVOACDOGALLERY_FRAMEGAL, &CMainFrame::OnAvoacdogalleryFramegal)
ON_COMMAND(ID_LAYOUT_BACKGROUNDCOLOR, &CMainFrame::OnLayoutBackgroundcolor)
ON_COMMAND(ID_LAYOUT_BACKGROUNDGRADIENT, &CMainFrame::OnLayoutBackgroundgradient)
ON_COMMAND(ID_LAYOUT_BACKGROUNDIMAGE, &CMainFrame::OnLayoutBackgroundimage)
ON_COMMAND(ID_AVOACDOGALLERY_LIGHTS, &CMainFrame::OnAvoacdogalleryLights)
ON_COMMAND(ID_LAYOUT_FRAM, &CMainFrame::OnLayoutFram)
ON_COMMAND(ID_LAYOUT_FRAMWTWOW, &CMainFrame::OnLayoutFramwtwow)
ON_COMMAND(ID_LAYOUT_FRAMETWOH, &CMainFrame::OnLayoutFrametwoh)
ON_COMMAND(ID_LAYOUT_FRAMEFOUR, &CMainFrame::OnLayoutFramefour)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_FRAMEFOUR, &CMainFrame::OnUpdateLayoutFramefour)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_FRAMETWOH, &CMainFrame::OnUpdateLayoutFrametwoh)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_FRAMWTWOW, &CMainFrame::OnUpdateLayoutFramwtwow)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_FRAM, &CMainFrame::OnUpdateLayoutFram)
ON_COMMAND(ID_LAYOUT_BACKGROUNDIMAGESELECT, &CMainFrame::OnLayoutBackgroundimageselect)
ON_COMMAND(ID_TEXTURE, &CMainFrame::OnTexture)
ON_COMMAND(ID_BUMP, &CMainFrame::OnBump)
ON_COMMAND(ID_LAYOUT_SLIDERTWO, &CMainFrame::OnLayoutSlidertwo)
ON_COMMAND(ID_LAYOUT_SLIDERONE, &CMainFrame::OnLayoutSliderone)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_NEWPAGE, &CMainFrame::OnUpdateLayoutNewpage)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_NEXTPAGE, &CMainFrame::OnUpdateLayoutNextpage)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_BACKPAGE, &CMainFrame::OnUpdateLayoutBackpage)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_SLIDERTWO, &CMainFrame::OnUpdateLayoutSlidertwo)
ON_UPDATE_COMMAND_UI(ID_LAYOUT_SLIDERONE, &CMainFrame::OnUpdateLayoutSliderone)
ON_COMMAND(ID_EXPORTPAGE_WAVEFRONT, &CMainFrame::OnExportpageWavefront)
ON_COMMAND(ID_EXPORTPAGE_JPEG, &CMainFrame::OnExportpageJpeg)
ON_COMMAND(ID_EXPORTPAGE_NVIDIA, &CMainFrame::OnExportpageNvidia)
ON_COMMAND(ID_EXPORTPAGE_BITMAP, &CMainFrame::OnExportpageBitmap)
ON_COMMAND(ID_FILE_COLLADA, &CMainFrame::OnFileCollada)
ON_COMMAND(ID_IMPORT_3DSTUDIO, &CMainFrame::OnImport3dstudio)
ON_COMMAND(ID_IMPORT_VRML, &CMainFrame::OnImportVrml)
ON_COMMAND(ID_IMPORT_WAVEFRONT, &CMainFrame::OnImportWavefront)
ON_COMMAND(ID_IMPORT_NVIDIAFORMAT, &CMainFrame::OnImportNvidiaformat)
ON_COMMAND(ID_AVOCDOMAIN_TOGGLEBROWS, &CMainFrame::OnAvocdomainTogglebrows)
ON_UPDATE_COMMAND_UI(ID_FILE_UPDATE, &CMainFrame::OnUpdateFileUpdate)
ON_COMMAND(ID_APPERANCE_MATERIAL11, &CMainFrame::OnApperanceMaterial11)
ON_COMMAND(ID_APPERANCE_MATERIAL12, &CMainFrame::OnApperanceMaterial12)
ON_COMMAND(ID_APPERANCE_MATERIAL16, &CMainFrame::OnApperanceMaterial16)
ON_COMMAND(ID_APPERANCE_MATERIAL17, &CMainFrame::OnApperanceMaterial17)
ON_COMMAND(ID_APPERANCE_MATERIAL20, &CMainFrame::OnApperanceMaterial20)
ON_COMMAND(ID_APPERANCE_MATERIAL30, &CMainFrame::OnApperanceMaterial30)
ON_COMMAND(ID_MP_DIFFUSE, &CMainFrame::OnMpDiffuse)
ON_UPDATE_COMMAND_UI(ID_MP_DIFFUSE, &CMainFrame::OnUpdateMpDiffuse)
ON_UPDATE_COMMAND_UI(ID_MP_EXPOSURE, &CMainFrame::OnUpdateMpExposure)
ON_UPDATE_COMMAND_UI(ID_MP_AMBIENT, &CMainFrame::OnUpdateMpAmbient)
ON_UPDATE_COMMAND_UI(ID_MP_FRENSELEXP, &CMainFrame::OnUpdateMpFrenselexp)
ON_UPDATE_COMMAND_UI(ID_MP_MAXREFL, &CMainFrame::OnUpdateMpMaxrefl)
ON_UPDATE_COMMAND_UI(ID_MP_MINREFL, &CMainFrame::OnUpdateMpMinrefl)
ON_UPDATE_COMMAND_UI(ID_MP_OPACITY, &CMainFrame::OnUpdateMpOpacity)
ON_UPDATE_COMMAND_UI(ID_MP_POLISHNESS, &CMainFrame::OnUpdateMpPolishness)
ON_UPDATE_COMMAND_UI(ID_MP_ROUGHNESS, &CMainFrame::OnUpdateMpRoughness)
ON_UPDATE_COMMAND_UI(ID_MP_SHININESS, &CMainFrame::OnUpdateMpShininess)
ON_UPDATE_COMMAND_UI(ID_MP_SPECULAR, &CMainFrame::OnUpdateMpSpecular)
ON_UPDATE_COMMAND_UI(ID_MP_BASECOLOR, &CMainFrame::OnUpdateMpBasecolor)
ON_COMMAND(ID_MP_BASECOLOR, &CMainFrame::OnMpBasecolor)
ON_COMMAND(ID_MP_AMBIENT, &CMainFrame::OnMpAmbient)
ON_COMMAND(ID_MP_EXPOSURE, &CMainFrame::OnMpExposure)
ON_COMMAND(ID_MP_FRENSELEXP, &CMainFrame::OnMpFrenselexp)
ON_COMMAND(ID_MP_SHININESS, &CMainFrame::OnMpShininess)
ON_COMMAND(ID_MP_OPACITY, &CMainFrame::OnMpOpacity)
ON_COMMAND(ID_MP_ROUGHNESS, &CMainFrame::OnMpRoughness)
ON_COMMAND(ID_MP_SPECULAR, &CMainFrame::OnMpSpecular)
ON_COMMAND(ID_MP_MINREFL, &CMainFrame::OnMpMinrefl)
ON_COMMAND(ID_MP_MAXREFL, &CMainFrame::OnMpMaxrefl)
ON_COMMAND(ID_MP_POLISHNESS, &CMainFrame::OnMpPolishness)
ON_COMMAND(ID_SLIDER, &CMainFrame::OnSlider)
ON_COMMAND(ID_SLIDER_ROUGHNESS, &CMainFrame::OnSliderRoughness)
ON_COMMAND(ID_SLIDER_SPECULAR, &CMainFrame::OnSliderSpecular)
ON_COMMAND(ID_SLIDER_AMBIENT, &CMainFrame::OnSliderAmbient)
ON_COMMAND(ID_SLIDER_OPACITY, &CMainFrame::OnSliderOpacity)
ON_COMMAND(ID_SLIDER_SHININESS, &CMainFrame::OnSliderShininess)
ON_UPDATE_COMMAND_UI(ID_SLIDER_AMBIENT, &CMainFrame::OnUpdateSliderAmbient)
ON_UPDATE_COMMAND_UI(ID_SLIDER, &CMainFrame::OnUpdateSlider)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPECULAR, &CMainFrame::OnUpdateSliderSpecular)
ON_UPDATE_COMMAND_UI(ID_SLIDER_OPACITY, &CMainFrame::OnUpdateSliderOpacity)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SHININESS, &CMainFrame::OnUpdateSliderShininess)
ON_UPDATE_COMMAND_UI(ID_SLIDER_ROUGHNESS, &CMainFrame::OnUpdateSliderRoughness)
ON_COMMAND(ID_SLIDER_REFLECTIVITY, &CMainFrame::OnSliderReflectivity)
ON_UPDATE_COMMAND_UI(ID_SLIDER_REFLECTIVITY, &CMainFrame::OnUpdateSliderReflectivity)
ON_COMMAND(ID_MP_REFLECTIVITY, &CMainFrame::OnMpReflectivity)
ON_UPDATE_COMMAND_UI(ID_MP_REFLECTIVITY, &CMainFrame::OnUpdateMpReflectivity)
ON_COMMAND(ID_SLIDER_MINREFL, &CMainFrame::OnSliderMinrefl)
ON_COMMAND(ID_SLIDER_MAXREFL, &CMainFrame::OnSliderMaxrefl)
ON_UPDATE_COMMAND_UI(ID_SLIDER_MAXREFL, &CMainFrame::OnUpdateSliderMaxrefl)
ON_UPDATE_COMMAND_UI(ID_SLIDER_MINREFL, &CMainFrame::OnUpdateSliderMinrefl)
ON_COMMAND(ID_SLIDER_FRESNEL_EXP, &CMainFrame::OnSliderFresnelExp)
ON_UPDATE_COMMAND_UI(ID_SLIDER_FRESNEL_EXP, &CMainFrame::OnUpdateSliderFresnelExp)
ON_COMMAND(ID_SLIDER_EXPOSURE, &CMainFrame::OnSliderExposure)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EXPOSURE, &CMainFrame::OnUpdateSliderExposure)
ON_COMMAND(ID_SLIDER_POLISHNESS, &CMainFrame::OnSliderPolishness)
ON_UPDATE_COMMAND_UI(ID_SLIDER_POLISHNESS, &CMainFrame::OnUpdateSliderPolishness)
ON_COMMAND(ID_SLIDER_RINGSCALE, &CMainFrame::OnSliderRingscale)
ON_UPDATE_COMMAND_UI(ID_SLIDER_RINGSCALE, &CMainFrame::OnUpdateSliderRingscale)
ON_COMMAND(ID_SLIDER_WOODSCALE1, &CMainFrame::OnSliderWoodscale1)
ON_UPDATE_COMMAND_UI(ID_SLIDER_WOODSCALE1, &CMainFrame::OnUpdateSliderWoodscale1)
ON_COMMAND(ID_SLIDER_WOODSCALE2, &CMainFrame::OnSliderWoodscale2)
ON_UPDATE_COMMAND_UI(ID_SLIDER_WOODSCALE2, &CMainFrame::OnUpdateSliderWoodscale2)
ON_COMMAND(ID_MP_ISWOOD, &CMainFrame::OnMpIswood)
ON_UPDATE_COMMAND_UI(ID_MP_ISWOOD, &CMainFrame::OnUpdateMpIswood)
ON_COMMAND(ID_SLIDER_BUMPHEIGHT, &CMainFrame::OnSliderBumpheight)
ON_COMMAND(ID_SLIDER_BUMPSCALE, &CMainFrame::OnSliderBumpscale)
ON_UPDATE_COMMAND_UI(ID_SLIDER_BUMPHEIGHT, &CMainFrame::OnUpdateSliderBumpheight)
ON_UPDATE_COMMAND_UI(ID_SLIDER_BUMPSCALE, &CMainFrame::OnUpdateSliderBumpscale)
ON_UPDATE_COMMAND_UI(ID_EDITBOX_BUMPFILE, &CMainFrame::OnUpdateEditboxBumpfile)
ON_COMMAND(IDC_STATIC, &CMainFrame::OnStatic)
ON_COMMAND(ID_BROWSE_BUMP, &CMainFrame::OnBrowseBump)
ON_COMMAND(ID_CHECKBOX_BUMPNORMAL, &CMainFrame::OnCheckboxBumpnormal)
ON_UPDATE_COMMAND_UI(ID_CHECKBOX_BUMPNORMAL, &CMainFrame::OnUpdateCheckboxBumpnormal)
ON_COMMAND(ID_AVOACDOGALLERY_BUMP, &CMainFrame::OnAvoacdogalleryBump)
ON_COMMAND(ID_AVOACDOGALLERY_WOODCOLOR, &CMainFrame::OnAvoacdogalleryWoodcolor)
ON_COMMAND(ID_AVOACDOGALLERY_WOODBUMP, &CMainFrame::OnAvoacdogalleryWoodbump)
ON_COMMAND(ID_MP_ENABLEBUMP, &CMainFrame::OnMpEnablebump)
ON_COMMAND(ID_MP_CLEARBUMP, &CMainFrame::OnMpClearbump)
ON_COMMAND(ID_SLIDER_WOODINTENSITY, &CMainFrame::OnSliderWoodintensity)
ON_UPDATE_COMMAND_UI(ID_SLIDER_WOODINTENSITY, &CMainFrame::OnUpdateSliderWoodintensity)
ON_COMMAND(ID_MP_BROWSETEXTURE, &CMainFrame::OnMpBrowsetexture)
ON_COMMAND(ID_AVOACDOGALLERY_TEXTURE, &CMainFrame::OnAvoacdogalleryTexture)
ON_COMMAND(ID_SLIDER_SCALEU, &CMainFrame::OnSliderScaleu)
ON_COMMAND(ID_SLIDER_SCALE_V, &CMainFrame::OnSliderScaleV)
ON_UPDATE_COMMAND_UI(ID_EDITBOX_TEXTUREFILE, &CMainFrame::OnUpdateEditboxTexturefile)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SCALE_V, &CMainFrame::OnUpdateSliderScaleV)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SCALEU, &CMainFrame::OnUpdateSliderScaleu)
ON_COMMAND(ID_MP_CLEARTEXTURE, &CMainFrame::OnMpCleartexture)
ON_COMMAND(ID_SLIDER_TEXOFFSETU, &CMainFrame::OnSliderTexoffsetu)
ON_COMMAND(ID_SLIDER_TEXOFFSETV, &CMainFrame::OnSliderTexoffsetv)
ON_UPDATE_COMMAND_UI(ID_SLIDER_TEXOFFSETV, &CMainFrame::OnUpdateSliderTexoffsetv)
ON_UPDATE_COMMAND_UI(ID_SLIDER_TEXOFFSETU, &CMainFrame::OnUpdateSliderTexoffsetu)
ON_COMMAND(ID_SLIDER_TEXANGLE, &CMainFrame::OnSliderTexangle)
ON_UPDATE_COMMAND_UI(ID_SLIDER_TEXANGLE, &CMainFrame::OnUpdateSliderTexangle)
ON_COMMAND(ID_MP_TEXTUREANGLE, &CMainFrame::OnMpTextureangle)
ON_UPDATE_COMMAND_UI(ID_MP_TEXTUREANGLE, &CMainFrame::OnUpdateMpTextureangle)
ON_COMMAND(ID_MP_ENABLESKIN, &CMainFrame::OnMpEnableskin)
ON_COMMAND(ID_MP_ENABLEHAIR, &CMainFrame::OnMpEnablehair)
ON_UPDATE_COMMAND_UI(ID_MP_ENABLESKIN, &CMainFrame::OnUpdateMpEnableskin)
ON_UPDATE_COMMAND_UI(ID_MP_ENABLEHAIR, &CMainFrame::OnUpdateMpEnablehair)
ON_COMMAND(ID_SLIDER_HAIR_LENGTH, &CMainFrame::OnSliderHairLength)
ON_UPDATE_COMMAND_UI(ID_SLIDER_HAIR_LENGTH, &CMainFrame::OnUpdateSliderHairLength)
ON_COMMAND(ID_SLIDER_HAIR_DENS, &CMainFrame::OnSliderHairDens)
ON_UPDATE_COMMAND_UI(ID_SLIDER_HAIR_DENS, &CMainFrame::OnUpdateSliderHairDens)
ON_COMMAND(ID_AVOACDOGALLERY_HAIR, &CMainFrame::OnAvoacdogalleryHair)
ON_COMMAND(ID_SLIDER_SKIN_SHIN, &CMainFrame::OnSliderSkinShin)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SKIN_SHIN, &CMainFrame::OnUpdateSliderSkinShin)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SKIN_OIL, &CMainFrame::OnUpdateSliderSkinOil)
ON_COMMAND(ID_SLIDER_SKIN_OIL, &CMainFrame::OnSliderSkinOil)
ON_UPDATE_COMMAND_UI(ID_MP_ENABLEBUMP, &CMainFrame::OnUpdateMpEnablebump)
ON_COMMAND(ID_MP_ENABLETEXTURE, &CMainFrame::OnMpEnabletexture)
ON_UPDATE_COMMAND_UI(ID_MP_ENABLETEXTURE, &CMainFrame::OnUpdateMpEnabletexture)
ON_COMMAND(ID_SLIDER_HEADLIGHT_L1, &CMainFrame::OnSliderHeadlightL1)
ON_COMMAND(ID_SLIDER_HEADLIGHT_L2, &CMainFrame::OnSliderHeadlightL2)
ON_COMMAND(ID_SLIDER_HEADLIGHT_L3, &CMainFrame::OnSliderHeadlightL3)
ON_UPDATE_COMMAND_UI(ID_SLIDER_HEADLIGHT_L1, &CMainFrame::OnUpdateSliderHeadlightL1)
ON_UPDATE_COMMAND_UI(ID_SLIDER_HEADLIGHT_L2, &CMainFrame::OnUpdateSliderHeadlightL2)
ON_UPDATE_COMMAND_UI(ID_SLIDER_HEADLIGHT_L3, &CMainFrame::OnUpdateSliderHeadlightL3)
ON_COMMAND(ID_AVOACDOGALLERY_LIGHT3, &CMainFrame::OnAvoacdogalleryLight3)
ON_COMMAND(ID_AVOACDOGALLERY_LIGHT2, &CMainFrame::OnAvoacdogalleryLight2)
ON_COMMAND(ID_AVOACDOGALLERY_LIGHT1, &CMainFrame::OnAvoacdogalleryLight1)
ON_COMMAND(ID_MP_SAVE_PRESET, &CMainFrame::OnMpSavePreset)
ON_COMMAND(ID_AVOCADOVIEW_MATERIAL, &CMainFrame::OnAvocadoviewMaterial)
ON_WM_SETCURSOR()
ON_COMMAND(ID_EDIT_PRESET_MATERIAL, &CMainFrame::OnEditPresetMaterial)
ON_COMMAND(ID_AVOACDOGALLERY_EDITBUMPPRESET, &CMainFrame::OnAvoacdogalleryEditbumppreset)
ON_COMMAND(ID_AVOACDOGALLERY_ADDBUMPGALLERY, &CMainFrame::OnAvoacdogalleryAddbumpgallery)
ON_COMMAND(ID_AVOACDOGALLERY_ADDTEXTUREGALLERY, &CMainFrame::OnAvoacdogalleryAddtexturegallery)
ON_COMMAND(ID_AVOACDOGALLERY_EDITTEXTUREPRESET, &CMainFrame::OnAvoacdogalleryEdittexturepreset)
ON_COMMAND(ID_AVOACDOGALLERY_ADDNEWBUMPGALLERY, &CMainFrame::OnAvoacdogalleryAddnewbumpgallery)
ON_COMMAND(ID_AVOACDOGALLERY_ADDNEWTEXTUREGALLERY, &CMainFrame::OnAvoacdogalleryAddnewtexturegallery)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETTEXTURE, &CMainFrame::OnAvoacdogalleryGalleryresettexture)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETBUMP, &CMainFrame::OnAvoacdogalleryGalleryresetbump)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETMATERIAL, &CMainFrame::OnAvoacdogalleryGalleryresetmaterial)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETWOOD, &CMainFrame::OnAvoacdogalleryGalleryresetwood)
ON_COMMAND(ID_AVOACDOGALLERY_ADDWOODGALLERY, &CMainFrame::OnAvoacdogalleryAddwoodgallery)
ON_COMMAND(ID_AVOACDOGALLERY_EDITWOODPRESET, &CMainFrame::OnAvoacdogalleryEditwoodpreset)
ON_COMMAND(ID_AVOACDOGALLERY_EDITSKINPRESET, &CMainFrame::OnAvoacdogalleryEditskinpreset)
ON_COMMAND(ID_AVOACDOGALLERY_ADDNEWSKINGALLERY, &CMainFrame::OnAvoacdogalleryAddnewskingallery)
ON_COMMAND(ID_AVOACDOGALLERY_EDITHAIRPRESET, &CMainFrame::OnAvoacdogalleryEdithairpreset)
ON_COMMAND(ID_AVOACDOGALLERY_ADDNEWHAIRGALLERY, &CMainFrame::OnAvoacdogalleryAddnewhairgallery)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETSKIN, &CMainFrame::OnAvoacdogalleryGalleryresetskin)
ON_COMMAND(ID_AVOACDOGALLERY_GALLERYRESETHAIR, &CMainFrame::OnAvoacdogalleryGalleryresethair)
ON_COMMAND(ID_AVOACDOGALLERY_SKIN, &CMainFrame::OnAvoacdogallerySkin)
ON_COMMAND(ID_LIGHTS_EDITLIGHTS, &CMainFrame::OnLightsEditlights)
ON_COMMAND(ID_LIGHTS_CLOSEEDITLIGHT, &CMainFrame::OnLightsCloseeditlight)
ON_COMMAND(ID_AVOCADOVIEW_MAKEGROUP, &CMainFrame::OnAvocadoviewMakegroup)
ON_UPDATE_COMMAND_UI(ID_AVOCADOVIEW_MAKEGROUP, &CMainFrame::OnUpdateAvocadoviewMakegroup)
ON_COMMAND(ID_INSERT_POLY, &CMainFrame::OnInsertPoly)
ON_COMMAND(ID_INSERT_CUBE, &CMainFrame::OnInsertCube)
ON_COMMAND(ID_INSERT_SPHERE, &CMainFrame::OnInsertSphere)
ON_COMMAND(ID_INSERT_PLANE, &CMainFrame::OnInsertPlane)
ON_COMMAND(ID_INSERT_CYLINDER, &CMainFrame::OnInsertCylinder)
ON_COMMAND(ID_INSERT_SIMPLECUBE, &CMainFrame::OnInsertSimplecube)
ON_COMMAND(ID_CHECKBOX_RECORDCAMERA, &CMainFrame::OnCheckboxRecordcamera)
ON_UPDATE_COMMAND_UI(ID_CHECKBOX_RECORDCAMERA, &CMainFrame::OnUpdateCheckboxRecordcamera)
ON_COMMAND(ID_INSERT_SPOTLIGHT, &CMainFrame::OnInsertSpotlight)
//ON_COMMAND(ID_INSERT_PATH, &CMainFrame::OnInsertPath)
ON_COMMAND(ID_INSERT_PATHLINE, &CMainFrame::OnInsertPathline)
ON_COMMAND(ID_SLIDER_SPECULAR2, &CMainFrame::OnSliderSpecular2)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPECULAR2, &CMainFrame::OnUpdateSliderSpecular2)
ON_UPDATE_COMMAND_UI(ID_FILE_IMPORT2, &CMainFrame::OnUpdateFileImport2)
ON_COMMAND(ID_AVOCADOVIEW_MAKEGROUP2, &CMainFrame::OnAvocadoviewMakegroup2)
ON_COMMAND(ID_AVOCADOVIEW_ADDTOGROUP, &CMainFrame::OnAvocadoviewAddtogroup)
ON_COMMAND(ID_VIEW_SHOW_ALL_PANES, &CMainFrame::OnViewShowAllPanes)
ON_COMMAND(ID_INSERT_PATHLINECOMPLEX, &CMainFrame::OnInsertPathlinecomplex)
ON_COMMAND(ID_INSERT_PATHLINE2, &CMainFrame::OnInsertPathline2)
ON_COMMAND(ID_INSERT_PROPSGALLERY, &CMainFrame::OnInsertPropsgallery)
ON_COMMAND(ID_INSERT_PROPSADD, &CMainFrame::OnInsertPropsadd)
ON_COMMAND(ID_INSERT_PROPSEDIT, &CMainFrame::OnInsertPropsedit)
ON_COMMAND(ID_INSERT_PROPSRESTORE, &CMainFrame::OnInsertPropsrestore)
ON_COMMAND(ID_LAYOUT_GOTOPAGEGALLERY, &CMainFrame::OnLayoutGotopagegallery)
ON_COMMAND(ID_LAYOUT_BACKGROUNIMAGEGALLRY, &CMainFrame::OnLayoutBackgrounimagegallry)
ON_COMMAND(ID_LAYOUT_BGIMAGEGALLEYADD, &CMainFrame::OnLayoutBgimagegalleyadd)
ON_COMMAND(ID_LAYOUT_BGIMAGEGALLERYEDIT, &CMainFrame::OnLayoutBgimagegalleryedit)
ON_COMMAND(ID_LAYOUT_BGIMAGEGALLERYRESTORE, &CMainFrame::OnLayoutBgimagegalleryrestore)
ON_COMMAND(ID_ROOM_CYLINDER, &CMainFrame::OnRoomCylinder)
ON_COMMAND(ID_ROOM_SPHERE, &CMainFrame::OnRoomSphere)
ON_COMMAND(ID_ROOM_CUDE, &CMainFrame::OnRoomCude)
ON_COMMAND(ID_ROOM_TOGGLESTATUSBAR, &CMainFrame::OnRoomTogglestatusbar)
ON_COMMAND(ID_SLIDER_ROOMSCALE_X, &CMainFrame::OnSliderRoomscaleX)
ON_COMMAND(ID_SLIDER_ROOMSCALE_Y, &CMainFrame::OnSliderRoomscaleY)
ON_COMMAND(ID_SLIDER_ROOMSCALE_Z, &CMainFrame::OnSliderRoomscaleZ)
ON_COMMAND(ID_SLIDER_GLOBAL_EXPOSURE, &CMainFrame::OnSliderGlobalExposure)
ON_UPDATE_COMMAND_UI(ID_SLIDER_GLOBAL_EXPOSURE, &CMainFrame::OnUpdateSliderGlobalExposure)
ON_UPDATE_COMMAND_UI(ID_SLIDER_ROOMSCALE_X, &CMainFrame::OnUpdateSliderRoomscaleX)
ON_UPDATE_COMMAND_UI(ID_SLIDER_ROOMSCALE_Y, &CMainFrame::OnUpdateSliderRoomscaleY)
ON_UPDATE_COMMAND_UI(ID_SLIDER_ROOMSCALE_Z, &CMainFrame::OnUpdateSliderRoomscaleZ)
ON_COMMAND(ID_SLIDER_LIGHT1_X, &CMainFrame::OnSliderLight1X)
ON_COMMAND(ID_SLIDER_LIGHT1_Y, &CMainFrame::OnSliderLight1Y)
ON_COMMAND(ID_SLIDER_LIGHT1_Z, &CMainFrame::OnSliderLight1Z)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT1_X, &CMainFrame::OnUpdateSliderLight1X)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT1_Y, &CMainFrame::OnUpdateSliderLight1Y)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT1_Z, &CMainFrame::OnUpdateSliderLight1Z)
ON_COMMAND(ID_SLIDER_LIGHT2_X, &CMainFrame::OnSliderLight2X)
ON_COMMAND(ID_SLIDER_LIGHT2_Y, &CMainFrame::OnSliderLight2Y)
ON_COMMAND(ID_SLIDER_LIGHT2_Z, &CMainFrame::OnSliderLight2Z)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT2_X, &CMainFrame::OnUpdateSliderLight2X)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT2_Y, &CMainFrame::OnUpdateSliderLight2Y)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT2_Z, &CMainFrame::OnUpdateSliderLight2Z)
ON_COMMAND(ID_SLIDER_LIGHT1_CUTOFF, &CMainFrame::OnSliderLight1Cutoff)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT1_CUTOFF, &CMainFrame::OnUpdateSliderLight1Cutoff)
ON_COMMAND(ID_CHECKBOX_LIGHT1_ENABLED, &CMainFrame::OnCheckboxLight1Enabled)
ON_UPDATE_COMMAND_UI(ID_CHECKBOX_LIGHT1_ENABLED, &CMainFrame::OnUpdateCheckboxLight1Enabled)
ON_COMMAND(ID_SLIDER_LIGHT2_CUTOFF, &CMainFrame::OnSliderLight2Cutoff)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT2_CUTOFF, &CMainFrame::OnUpdateSliderLight2Cutoff)
ON_COMMAND(ID_CHECKBOX_LIGHT2_ENABLED, &CMainFrame::OnCheckboxLight2Enabled)
ON_UPDATE_COMMAND_UI(ID_CHECKBOX_LIGHT2_ENABLED, &CMainFrame::OnUpdateCheckboxLight2Enabled)
ON_COMMAND(ID_SLIDER_LIGHT3_X, &CMainFrame::OnSliderLight3X)
ON_COMMAND(ID_SLIDER_LIGHT3_Y, &CMainFrame::OnSliderLight3Y)
ON_COMMAND(ID_SLIDER_LIGHT3_Z, &CMainFrame::OnSliderLight3Z)
ON_COMMAND(ID_SLIDER_LIGHT3_CUTOFF, &CMainFrame::OnSliderLight3Cutoff)
ON_COMMAND(ID_CHECKBOX_LIGHT3_ENABLED, &CMainFrame::OnCheckboxLight3Enabled)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT3_X, &CMainFrame::OnUpdateSliderLight3X)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT3_Y, &CMainFrame::OnUpdateSliderLight3Y)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT3_Z, &CMainFrame::OnUpdateSliderLight3Z)
ON_UPDATE_COMMAND_UI(ID_SLIDER_LIGHT3_CUTOFF, &CMainFrame::OnUpdateSliderLight3Cutoff)
ON_UPDATE_COMMAND_UI(ID_CHECKBOX_LIGHT3_ENABLED, &CMainFrame::OnUpdateCheckboxLight3Enabled)
ON_COMMAND(ID_LAYOUT_GLOBALAMBIENTCOLOR, &CMainFrame::OnLayoutGlobalambientcolor)
ON_COMMAND(ID_LIGHTS_LIGHT1AMBIENT, &CMainFrame::OnLightsLight1ambient)
ON_COMMAND(ID_LIGHTS_LIGHT1DIFFUSE, &CMainFrame::OnLightsLight1diffuse)
ON_COMMAND(ID_LIGHTS_LIGHT1SPECULAR, &CMainFrame::OnLightsLight1specular)
ON_COMMAND(ID_LIGHTS_LIGHT2AMBIENT, &CMainFrame::OnLightsLight2ambient)
ON_COMMAND(ID_LIGHTS_LIGHT2DIFFUSE, &CMainFrame::OnLightsLight2diffuse)
ON_COMMAND(ID_LIGHTS_LIGHT2SPECULAR, &CMainFrame::OnLightsLight2specular)
ON_COMMAND(ID_LIGHTS_LIGHT3AMBIENT, &CMainFrame::OnLightsLight3ambient)
ON_COMMAND(ID_LIGHTS_LIGHT3DIFFUSE, &CMainFrame::OnLightsLight3diffuse)
ON_COMMAND(ID_LIGHTS_LIGHT3SPECULAR, &CMainFrame::OnLightsLight3specular)
ON_COMMAND(ID_LIGHTS_SPOTAMBIENT, &CMainFrame::OnLightsSpotambient)
ON_COMMAND(ID_LIGHTS_SPOTDIFFUSE, &CMainFrame::OnLightsSpotdiffuse)
ON_COMMAND(ID_LIGHTS_SPOTSPECULAR, &CMainFrame::OnLightsSpotspecular)
ON_COMMAND(ID_SLIDER_SPOT_ATTUNUATION, &CMainFrame::OnSliderSpotAttunuation)
ON_COMMAND(ID_SLIDER_SPOT_INTENSITY, &CMainFrame::OnSliderSpotIntensity)
ON_COMMAND(ID_SLIDER_SPOT_EXPONENT, &CMainFrame::OnSliderSpotExponent)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPOT_EXPONENT, &CMainFrame::OnUpdateSliderSpotExponent)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPOT_ATTUNUATION, &CMainFrame::OnUpdateSliderSpotAttunuation)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPOT_INTENSITY, &CMainFrame::OnUpdateSliderSpotIntensity)
ON_COMMAND(ID_SLIDER_SPOT_LINEAR, &CMainFrame::OnSliderSpotLinear)
ON_COMMAND(ID_SLIDER_SPOT_QUADRIC, &CMainFrame::OnSliderSpotQuadric)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPOT_LINEAR, &CMainFrame::OnUpdateSliderSpotLinear)
ON_UPDATE_COMMAND_UI(ID_SLIDER_SPOT_QUADRIC, &CMainFrame::OnUpdateSliderSpotQuadric)
ON_COMMAND(ID_SLIDER_IOR, &CMainFrame::OnSliderIor)
ON_UPDATE_COMMAND_UI(ID_SLIDER_IOR, &CMainFrame::OnUpdateSliderIor)
ON_UPDATE_COMMAND_UI(ID_ROOM_CYLINDER, &CMainFrame::OnUpdateRoomCylinder)
ON_UPDATE_COMMAND_UI(ID_ROOM_CUDE, &CMainFrame::OnUpdateRoomCude)
ON_UPDATE_COMMAND_UI(ID_ROOM_SPHERE, &CMainFrame::OnUpdateRoomSphere)
ON_COMMAND(ID_FILE_OPTIONS, &CMainFrame::OnFileOptions)
ON_COMMAND(ID_AVOCADOVIEW_MAKEWELD, &CMainFrame::OnAvocadoviewMakeweld)
ON_COMMAND(ID_MATERIALSTATE_COMBO, &CMainFrame::OnMaterialstateCombo)
ON_COMMAND(ID_MATERIALSTATE_ADD, &CMainFrame::OnMaterialstateAdd)
ON_UPDATE_COMMAND_UI(ID_MATERIALSTATE_ADD, &CMainFrame::OnUpdateMaterialstateAdd)
ON_COMMAND(ID_MATERIALSTATE_DELETE, &CMainFrame::OnMaterialstateDelete)
ON_UPDATE_COMMAND_UI(ID_MATERIALSTATE_DELETE, &CMainFrame::OnUpdateMaterialstateDelete)
ON_COMMAND(ID_MATERIALSTATE_ADDEXISTING, &CMainFrame::OnMaterialstateAddexisting)
ON_UPDATE_COMMAND_UI(ID_MATERIALSTATE_ADDEXISTING, &CMainFrame::OnUpdateMaterialstateAddexisting)
ON_COMMAND(ID_SLIDER_EFFECT_BRIGHT, &CMainFrame::OnSliderEffectBright)
ON_COMMAND(ID_SLIDER_EFFECT_CONT, &CMainFrame::OnSliderEffectCont)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_BRIGHT, &CMainFrame::OnUpdateSliderEffectBright)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_CONT, &CMainFrame::OnUpdateSliderEffectCont)
ON_COMMAND(ID_SLIDER_EFFECT_HUE, &CMainFrame::OnSliderEffectHue)
ON_COMMAND(ID_SLIDER_EFFECT_SATURATE, &CMainFrame::OnSliderEffectSaturate)
ON_COMMAND(ID_SLIDER_EFFECT_NOISE, &CMainFrame::OnSliderEffectNoise)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_NOISE, &CMainFrame::OnUpdateSliderEffectNoise)
ON_COMMAND(ID_SLIDER_EFFECT_SOFTEN, &CMainFrame::OnSliderEffectSoften)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_SOFTEN, &CMainFrame::OnUpdateSliderEffectSoften)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_HUE, &CMainFrame::OnUpdateSliderEffectHue)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_SATURATE, &CMainFrame::OnUpdateSliderEffectSaturate)
ON_COMMAND(ID_CHECK_EFFECT_SATURATE, &CMainFrame::OnCheckEffectSaturate)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_SATURATE, &CMainFrame::OnUpdateCheckEffectSaturate)
ON_COMMAND(ID_CHECK_EFFECT_BRIGHT, &CMainFrame::OnCheckEffectBright)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_BRIGHT, &CMainFrame::OnUpdateCheckEffectBright)
ON_COMMAND(ID_CHECK_EFFECT_NOISE, &CMainFrame::OnCheckEffectNoise)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_NOISE, &CMainFrame::OnUpdateCheckEffectNoise)
ON_COMMAND(ID_CHECK_EFFECT_SOFTEN, &CMainFrame::OnCheckEffectSoften)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_SOFTEN, &CMainFrame::OnUpdateCheckEffectSoften)
ON_COMMAND(ID_BUTTON_EFFECT_POST, &CMainFrame::OnButtonEffectPost)
ON_UPDATE_COMMAND_UI(ID_BUTTON_EFFECT_POST, &CMainFrame::OnUpdateButtonEffectPost)
ON_COMMAND(ID_CHECK_EFFECT_AO, &CMainFrame::OnCheckEffectAo)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_AO, &CMainFrame::OnUpdateCheckEffectAo)
ON_COMMAND(ID_CHECK_EFFECT_GAMMA, &CMainFrame::OnCheckEffectGamma)
ON_COMMAND(ID_CHECK_EFFECT_TONEMAP, &CMainFrame::OnCheckEffectTonemap)
ON_COMMAND(ID_CHECK_EFFECT_FXAA, &CMainFrame::OnCheckEffectFxaa)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_GAMMA, &CMainFrame::OnUpdateCheckEffectGamma)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_TONEMAP, &CMainFrame::OnUpdateCheckEffectTonemap)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_FXAA, &CMainFrame::OnUpdateCheckEffectFxaa)
ON_COMMAND(ID_CHECK_EFFECT_DOF, &CMainFrame::OnCheckEffectDof)
ON_UPDATE_COMMAND_UI(ID_CHECK_EFFECT_DOF, &CMainFrame::OnUpdateCheckEffectDof)
ON_COMMAND(ID_SLIDER_EFFECT_DOFPLANE, &CMainFrame::OnSliderEffectDofplane)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_DOFPLANE, &CMainFrame::OnUpdateSliderEffectDofplane)
ON_COMMAND(ID_SLIDER_EFFECT_GAMMA, &CMainFrame::OnSliderEffectGamma)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_GAMMA, &CMainFrame::OnUpdateSliderEffectGamma)
ON_COMMAND(ID_SLIDER_EFFECT_TONEMAP, &CMainFrame::OnSliderEffectTonemap)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_TONEMAP, &CMainFrame::OnUpdateSliderEffectTonemap)
ON_COMMAND(ID_SLIDER_EFFECT_AOINTENSITY, &CMainFrame::OnSliderEffectAointensity)
ON_UPDATE_COMMAND_UI(ID_SLIDER_EFFECT_AOINTENSITY, &CMainFrame::OnUpdateSliderEffectAointensity)
ON_COMMAND(ID_INSERT_CALLOUT, &CMainFrame::OnInsertCallout)
ON_COMMAND(ID_EXPORTPAGE_PNG, &CMainFrame::OnExportpagePng)
ON_COMMAND(ID_EXPORTPAGE_TIFF, &CMainFrame::OnExportpageTiff)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

void CMainFrame::OnAddSectionPlane()
{
	
	if (this->GetActiveDocument())
	{
		int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
		
		GetActiveAvocadoDoc()->AvocadoInvokeDoc ("AddAnnotationElement",id,"string AnnotationType=SectionPlane;");
	}
}
void CMainFrame::OnClassViewSelection(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (GetActiveAvocadoView() == 0)
	 return ;
	int id = m_wndClassView.GetSelectedID ();
	bool shift = m_wndClassView.GetLastShift();
	int oldID = GetActiveAvocadoView()->GetSelection();
	int viewID = GetActiveAvocadoView()->GetID();
	string shiftstr = shift ? "1" : "0";
	if (id != oldID )
	{
		if (id >= 0)
		{
		string pr;
		std::stringstream pStr;
		pStr <<   "string owner=ImportModule"
			 <<  ",bool prePick=false" 
			 <<  ",int eid=" << id
			 <<  ",int vid=" << viewID 
			 << ",bool multi=" << shiftstr << ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("OnPick",GetActiveDocID (),pStr.str().c_str());
		}
		else
		{
			string pr;
			std::stringstream pStr;
			pStr <<  "bool prePick=false," 
			 <<  "int vid=" << viewID 
			 << ";";
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("OnPickNothing",GetActiveDocID (),pStr.str().c_str());
	
		}
	}
}

void CMainFrame::OnUpdateAddSectionPlane(CCmdUI *pCmdUI)
{
	
	pCmdUI->Enable(true);
}
CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_clipboard = -1;
	m_waitCommand = -1;
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_WINDOWS_7);
}

CMainFrame::~CMainFrame()
{
}
BOOL CMainFrame::OnViewChange(UINT nCmdID)
// There is an ON_COMMAND_RANGE message map entry associated with
// OnViewChange:
// ON_COMMAND_RANGE( ID_VIEW_VIEW1, ID_VIEW_VIEW2, OnViewChange)
{
	return true;
}

#if 0
// tried this for the OLE in-place menus , I think..
//should be deleted as soon as OLE inplace menus stabalize.
static  int FindMenuItem(CMenu* Menu, LPCTSTR MenuString)
 {
 ASSERT(Menu);
 ASSERT(::IsMenu(Menu->GetSafeHmenu()));
 
int count = Menu->GetMenuItemCount();
 for (int i = 0; i < count; i++)
 {
 CString str;
 if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
 (StrCmpW(str, MenuString) == 0))
 return i;
 }
 
return -1;
 }
int CMainFrame::RemoveUpdateButton ()
{
 	CMenu *pMenu = ((CMainFrame*)AfxGetMainWnd())->GetMenu();
 
		int iPos = FindMenuItem(pMenu,L"&File");
		 if(iPos != -1)
		 {
			CMenu* pSubmenu = pMenu->GetSubMenu(iPos);
 
			pSubmenu->RemoveMenu(ID_FILE_UPDATE, MF_BYCOMMAND);
		 }
}
#endif
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bIsViewerMode = (theApp.GetAppMode () == CAvocadoApp::VIEWER_APP);
	BOOL bNameValid;
	// set the visual manager and style based on persisted value
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	if (!bIsViewerMode)
	{
		// Authoring ribbon..
		m_wndRibbonBar.LoadFromResource(IDR_RIBBON);
		
		//CMFCRibbonApplicationButton *appb = m_wndRibbonBar.GetApplicationButton ();
		//appb->RemoveSubItem (ID_FILE_UPDATE);
		//HMENU appmenu = appb->GetMenu ();
		//RemoveMenu (appmenu,ID_FILE_UPDATE,MF_BYCOMMAND );

	
		{
			CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
			this->m_wndRibbonBar.GetElementsByID (ID_MATERIAL_COLOR,arr);
			CMFCRibbonBaseElement *el = arr.GetAt (0);
			CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
			colel->EnableOtherButton (_T("More.."),_T("Choose more colors"));

			// Texture gallery..
			CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
			this->m_wndRibbonBar.GetElementsByID (ID_AVOCADOVIEW_MATERIAL,arr2);
			CMFCRibbonBaseElement *elb = arr2.GetAt (0);
			CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
			galel->SetAlwaysLargeImage (true);
			galel->EnableMenuResize (TRUE,FALSE);
			//galel->AddGroup (L"Group",IDB_BITMAP5);
			galel->SetIconsInRow (3);
			theApp.m_galleryManager.RegisterGallery (CString ("Material Presets"),ID_AVOCADOVIEW_MATERIAL,galel,CString("material_preset.xml"),CString (_T("gallery_default_material.bmp")),IDB_BITMAP17);

			// bumps gallery
			
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_BUMP,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Bump Map Presets"),ID_AVOACDOGALLERY_BUMP,galel,CString("bump_preset.xml"),CString(_T("gallery_default_bump.bmp")),IDB_BITMAP17);
	arr2.RemoveAll ();
			//texture gallery
			
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_TEXTURE,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Texture Presets"),ID_AVOACDOGALLERY_TEXTURE,galel,CString("texture_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			//wood bumps gallery
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_WOODBUMP,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Wood Bump Presets"),ID_AVOACDOGALLERY_WOODBUMP,galel,CString("wood_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			//hair textures gallery
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_HAIR,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Hair Presets"),ID_AVOACDOGALLERY_HAIR,galel,CString("hair_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			//skin textures gallery
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_SKIN,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Skin Presets"),ID_AVOACDOGALLERY_SKIN,galel,CString("skin_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			//props gallery
			this->m_wndRibbonBar.GetElementsByID (ID_INSERT_PROPSGALLERY,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Props Presets"),ID_INSERT_PROPSGALLERY,galel,CString("props_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			//bg image gallery
			this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_BACKGROUNIMAGEGALLRY,arr2);
			elb = arr2.GetAt (0);
			galel = (CMFCRibbonGallery*)elb;
			theApp.m_galleryManager.RegisterGallery (CString ("Environment background Presets"),ID_LAYOUT_BACKGROUNIMAGEGALLRY,galel,CString("envbg_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
			arr2.RemoveAll ();
			
		}

		
	}
	else
	{
		// Viewer ribbon..
		m_wndRibbonBar.LoadFromResource(IDR_RIBBON1);
		m_wndRibbonBar.ToggleMimimizeState();

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
			this->m_wndRibbonBar.GetElementsByID (ID_MATERIAL_COLOR,arr);
			CMFCRibbonBaseElement *el = arr.GetAt (0);
			CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
			colel->EnableOtherButton (L"More..",L"Choose more colors");
		//bg image gallery
					CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		
			CMFCRibbonBaseElement *elb ;//= arr2.GetAt (0);
			CMFCRibbonGallery *galel ;//= (CMFCRibbonGallery*)elb;
		this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_BACKGROUNIMAGEGALLRY,arr2);
		elb = arr2.GetAt (0);
		galel = (CMFCRibbonGallery*)elb;
		theApp.m_galleryManager.RegisterGallery (CString ("Environment background Presets"),ID_LAYOUT_BACKGROUNIMAGEGALLRY,galel,CString("envbg_preset.xml"),CString (_T("gallery_default_texture.bmp")),IDB_BITMAP17);
		arr2.RemoveAll ();
	}
	if (!bIsViewerMode)
	{
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CString strTitlePane1;
	CString strTitlePane2;
	CString strTitlePane3;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	bNameValid = strTitlePane3.LoadString(IDS_STATUS_PANE3);
	ASSERT(bNameValid);

	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE3, strTitlePane3, TRUE), strTitlePane3);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonProgressBar(ID_STATUSBAR_PROGRESS, 70, 18), strTitlePane2);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonComboBox(ID_STATUSBAR_COMBO, FALSE, 46), strTitlePane2);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);
	}
	// enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// Navigation pane will be created at left, so temporary disable docking at the left side:
	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM | CBRS_ALIGN_RIGHT);

	if (!bIsViewerMode)
	{
	// Create and setup "Outlook" navigation bar:
	if (!CreateOutlookBar(m_wndNavigationBar, ID_VIEW_NAVIGATION, m_wndTree, m_wndCalendar, 400))
	{
		TRACE0("Failed to create navigation pane\n");
		return -1;      // fail to create
	}
	
	// Create a caption bar:
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	}
	}
	// Outlook bar is created and docking on the left side should be allowed.
	EnableDocking(CBRS_ALIGN_LEFT);
	EnableAutoHidePanes(CBRS_ALIGN_RIGHT);

	// Load menu item image (not placed on any standard toolbars):
	CMFCToolBar::AddToolBarForImageCollection(IDR_MENU_IMAGES, theApp.m_bHiColorIcons ? IDB_MENU_IMAGES_24 : 0);

	// create docking windows
	if (!CreateDockingWindows())
	{
		TRACE0("Failed to create docking windows\n");
		return -1;
	}
	
	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	
	BOOL res = m_wndSplitter. Create(this,
		2, 2,               // TODO: adjust the number of rows, columns
		CSize(10, 10),      // TODO: adjust the minimum pane size
		pContext);
	/*
	BOOL res = m_wndSplitter. CreateStatic(this,
		1, 1);
	 m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAvocadoView), CSize(0,0), 
      pContext);*/
		m_wndSplitter.SetScrollStyle (0);
		
	return res;
} 

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	cs.style &= ~(WS_HSCROLL|WS_VSCROLL);
	
	return TRUE;
}
bool CMainFrame::ImportFile (LPCTSTR lpszFileName)
{
	return (GetActiveAvocadoDoc ()->ImportFile (lpszFileName));
}

BOOL CMainFrame::OnRenameElement ()
{
if (GetActiveAvocadoView() == 0)
	 return FALSE;
	int id = m_wndClassView.GetSelectedID ();
	char idstr[10] ;
	itoa(id,idstr,10);
	CString newName = m_wndClassView.GetLastString  ();
	
	if (newName == CString (""))
	{
		//AfxMessageBox (_T("Please choose a valid name."));
		return TRUE;
	}
	wstring wnewname (newName.GetString ());
	char newnamechar [200];
	::wcstombs (newnamechar,wnewname.c_str(),200);

	string parms = "int eid=";
	parms+= string (idstr);
	parms+= ",string newname=";
	parms += string (newnamechar);
	parms += ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc ("RenameElement",GetActiveAvocadoDoc()->GetAvoID (),parms.c_str());
	return TRUE;
}
BOOL CMainFrame::CreateDockingWindows(bool create)
{
	BOOL bNameValid;

	//LPRECT rect;
	//GetClientRect (rect);
	// Create class view
	if (create)
	{
		CString strClassView;
		bNameValid = strClassView.LoadString(IDS_CLASS_VIEW);
		ASSERT(bNameValid);
		if (!m_wndClassView.Create(strClassView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_CLASSVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
		{
			TRACE0("Failed to create Class View window\n");
			return FALSE; // failed to create
		}
		// Create web view
		CString strWebView;
		bNameValid = strWebView.LoadString(IDS_HTML_VIEW);
		ASSERT(bNameValid);
		if (!m_wndWebView.Create(strWebView, this, CRect(0, 0, 280, 200), TRUE, ID_VIEW_HTMLVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
		{
			TRACE0("Failed to create Html Editor window\n");
			return FALSE; // failed to create
		}
	}
//	if (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP)
	{
		m_wndClassView.ShowPane(true,false,false);
		m_wndWebView.ShowPane(true,false,false);
		
	}
//	else
	{
		
	}
	

	m_wndWebView.EnableDocking(CBRS_ALIGN_ANY);
	m_wndClassView.EnableDocking(CBRS_ALIGN_ANY);
	
	
	if (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP)
	{
		if (create)
		{
			// Create file view
			CString strFileView;
			bNameValid = strFileView.LoadString(IDS_FILE_VIEW);
			ASSERT(bNameValid);
			if (!m_wndFileView.Create(strFileView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_FILEVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT| CBRS_FLOAT_MULTI))
			{
				TRACE0("Failed to create File View window\n");
				return FALSE; // failed to create
			}
		}
		m_wndFileView.ShowPane(true,false,false);

		m_wndFileView.EnableDocking(CBRS_ALIGN_ANY);
		if (create)
		{
			DockPane(&m_wndFileView);
			DockPane(&m_wndWebView,AFX_IDW_DOCKBAR_BOTTOM);
			CDockablePane* pTabbedBar = NULL;
			m_wndClassView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar);
		}
	}
	else
	{
		CDockablePane* pTabbedBar2 = NULL;
		if (create)
		{
		//m_wndWebView.AttachToTabWnd(&m_wndFileView, DM_SHOW, TRUE, &pTabbedBar2);
			DockPane(&m_wndWebView);
			DockPane(&m_wndClassView);
		}
		m_wndWebView.SetAutoHideMode (TRUE,CBRS_LEFT);
		m_wndClassView.SetAutoHideMode (TRUE,CBRS_LEFT);
		
		// Create shell view
		if (create)
		{
			CString strShellView = CString (L"Shell");
			//bNameValid = strClassV);
			ASSERT(bNameValid);
			if (!m_wndShellView.Create(strShellView, this, CRect(0, 0, 200, 200), TRUE, ID_VIEW_SHELLVIEW, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
			{
				TRACE0("Failed to create Shell View window\n");
				return FALSE; // failed to create
			}
		}
		m_wndShellView.ShowPane (true,false,false);
		m_wndShellView.EnableDocking(CBRS_ALIGN_ANY);

		m_wndShellView.SetAutoHideMode (TRUE,CBRS_LEFT);
		//m_wndFileView.SetAutoHideMode (TRUE,CBRS_LEFT);
		//m_wndWebView.ToggleAutoHide ();
		//m_wndClassView.ToggleAutoHide();
		
	}
	
SetDockingWindowIcons(theApp.m_bHiColorIcons);
	

	return TRUE;
}
CAvocadoDoc* CMainFrame::GetActiveAvocadoDoc()
{
	if (GetActiveDocument())
		return ((CAvocadoDoc*)GetActiveDocument ());
	else
	{
		
	}
	return false;
}
	void CMainFrame::RefreshActiveDocument ()
	{
		// its possible to reach here before we have an active view.
		if (GetActiveAvocadoDoc())
		{
			// Update element pane..
			avocado::AvocadoElementsList elist ;
			GetActiveAvocadoDoc()->GetElements(elist);
			m_wndClassView.ClearList ();

			vector<int> try_again_list;
			// two passes. One for fathers, second for children. so simple..
			for (size_t i=0; i< elist.size();i++)
			{
				if (elist[i].parentID < 0)
					m_wndClassView.AddElement ( elist[i].name,elist[i].id,elist[i].isVisible,elist[i].parentID );	
			}
			for (size_t i=0; i< elist.size();i++)
			{
					if (elist[i].parentID >= 0)
						if (m_wndClassView.AddElement ( elist[i].name,elist[i].id,elist[i].isVisible,elist[i].parentID ) == false)
						try_again_list.push_back (int(i));
			}
			// here we go over and over again on the hirearchy until filled..
			// the problem is that we must firs add the parents and then children.
			// can be surely optimized;..
			while (try_again_list.size() > 0)
			{
				vector<int> temp_list;
				for (size_t ti=0; ti< try_again_list.size();ti++)
				{
					int idx = try_again_list[ti];
						if (elist[idx].parentID >= 0)
							if (m_wndClassView.AddElement ( elist[idx].name,elist[idx].id,elist[idx].isVisible,elist[idx].parentID ) == false)
							temp_list.push_back (idx);
				}
				try_again_list = temp_list;
			}
			// Update file view pane..
			if  (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP)
			{
				avocado::AvocadoFilesList flist ;
				GetActiveAvocadoDoc()->GetFiles(flist);
				m_wndFileView.ClearList ();
				for (size_t i=0; i< flist.size();i++)
				{
					if (flist[i].m_fileName.size ())
						m_wndFileView.AddFileLink ( flist[i].m_fileName);//,elist[i].id,elist[i].isVisible );
				}
			}

			// We have elements so release the caption bar hint..
			// Later on we should take more advantage of caption bar..its good stuff.
			if (elist.size())
			{
				if (m_wndCaptionBar)
				m_wndCaptionBar.ShowPane (FALSE,FALSE,FALSE);
				// update UI ??
			}
			else
			{
				if (m_wndCaptionBar)
				m_wndCaptionBar.ShowPane (TRUE,FALSE,FALSE);
			}
		}
	}
	void CMainFrame::ShowCaptionBar ()
	{
		m_wndCaptionBar.ShowPane (TRUE,FALSE,FALSE);
	}
	void CMainFrame::RefreshViewSelection ()
	{
		int sel = GetActiveAvocadoView()->GetSelection ();
		if (GetActiveAvocadoView()->GetSelectionCount () > 1)
			sel = -1;
		std::vector<int> sellist ;
		for (int selIdx=0; selIdx < GetActiveAvocadoView()->GetSelectionCount () ;selIdx++)
			sellist.push_back (GetActiveAvocadoView()->GetSelection (selIdx));
		m_wndClassView.HighlightElements (sellist);

		unsigned int context = 0;
		if (sel > -1)
		{
			context = GetActiveAvocadoDoc ()->GetElementContext(sel);
			

			std::string htmlText = GetActiveAvocadoDoc()->GetElementMetaData (sel,"Html");
			std::string selectedName = "";
			GetActiveAvocadoDoc()->GetElementName  (sel,selectedName);
			
//	htmlText = "<html><body><button onclick=\"javascript:window.external.SendAvocadoMsg('FitToPage','int MikeParam=2;');\">Hello</button> </body></html>";
	
			m_wndWebView.SetCurrentHtml (htmlText);
			TCHAR wname[300];
#ifdef UNICODE
			::mbstowcs (wname,selectedName.c_str(),300);
#else
			::strcpy (wname,selectedName.c_str());
#endif
			m_wndWebView.SetWindowText (wname);
			if (theApp.GetAppMode () == CAvocadoApp::VIEWER_APP)
				m_wndWebView.SetAutoHideMode (FALSE,CBRS_LEFT);

			if (m_wndStatusBar)
			{
				CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> elarr;
				CMFCRibbonStatusBarPane* el = (CMFCRibbonStatusBarPane*)(m_wndStatusBar.FindElement(ID_STATUSBAR_PANE1));
				if (el)
				{
					el->SetText (CString (L"Selected item: ") + CString(wname));
					m_wndStatusBar.RecalcLayout ();
					m_wndStatusBar.RedrawWindow();
				}
				//m_wndStatusBar.RedrawWindow();
			}
			if (m_waitCommand > -1 )
			{
				if (m_waitCommand == ID_AVOCADOVIEW_MATERIAL)
				{
					vector<int> selList;
					selList.push_back(sel);
					SetElementsMaterial (selList,m_waitData);
			
					m_waitCommand = -1;
				}
			}
		} 
		else
		{

			//m_wndWebView.SetWindowTextW (L"Details");
			CString PageIndicatorString;
			int currentVS = GetActiveAvocadoDoc()->GetCurrentViewState ();
			int countVS = int(GetActiveAvocadoDoc()->GetViewStateCount());
			
			char s_pagecount [30];
			char s_pagecurrent [30];
			// ui count starts from 1.. 
			itoa (currentVS+1,s_pagecurrent,10);
			itoa  (countVS,s_pagecount,10);
			string stitle = "Page" +string (s_pagecurrent) +"/" + string (s_pagecount);
			wchar_t wtitle[200];
			::mbstowcs (wtitle,stitle.c_str(),200);
	
			// set this behaviour as option. // AvocadoOption
			//if (theApp.GetAppMode () == CAvocadoApp::VIEWER_APP)
				//m_wndWebView.SetAutoHideMode (TRUE,CBRS_LEFT);
			if (GetActiveDocument())
			{
				if  (! ((COleServerDoc*)GetActiveDocument())->IsEmbedded ())
				{
					m_wndWebView.SetWindowTextW (wtitle);
					std::string htmlText = GetActiveAvocadoDoc()->GetViewStateHtml (currentVS);
					if (htmlText.size())// != string (""));
						m_wndWebView.SetCurrentHtml (htmlText);
				}
			} else
			{
				m_wndWebView.SetWindowTextW (L"Details");
			}
			if (m_wndStatusBar)
			{
				CMFCRibbonStatusBarPane* el = (CMFCRibbonStatusBarPane*)(m_wndStatusBar.FindElement(ID_STATUSBAR_PANE1));
				if (el)
				{
					CString cstatus(L"Nothing selected");
					cstatus += L", ";
					cstatus += wtitle;
					el->SetText (cstatus);
					m_wndStatusBar.RecalcLayout ();
					m_wndStatusBar.RedrawWindow();
				}
			//	m_wndStatusBar.RedrawWindow();
			}
		}

	
		if (theApp.GetAppMode () != CAvocadoApp::VIEWER_APP)
		{
			m_wndRibbonBar.ShowContextCategories (ID_CONTEXT2,context == ID_CONTEXT2);
			m_wndRibbonBar.ShowContextCategories (ID_CONTEXT_LINE_STYLE,context == ID_CONTEXT_LINE_STYLE);
			m_wndRibbonBar.ShowContextCategories (ID_CONTEXT_ANNOTATIONS,context == ID_CONTEXT_ANNOTATIONS);
			m_wndRibbonBar.ShowContextCategories (ID_CONTEXT_ANNOTATION_SPOT,context == ID_CONTEXT_ANNOTATION_SPOT);

			if (context == ID_CONTEXT_ANNOTATION_SPOT)
				m_wndRibbonBar.ActivateContextCategory (ID_CONTEXT_ANNOTATION_SPOT);
			else if (context == ID_CONTEXT_ANNOTATIONS)
				m_wndRibbonBar.ActivateContextCategory (ID_CONTEXT_ANNOTATIONS);
			else if (context == ID_CONTEXT_LINE_STYLE)
				m_wndRibbonBar.ActivateContextCategory (ID_CONTEXT_LINE_STYLE);
			else if (context == ID_CONTEXT2)
				m_wndRibbonBar.ActivateContextCategory (context == ID_CONTEXT2);

			m_wndRibbonBar.RecalcLayout ();
		}
		//m_wndRibbonBar.ActivateContextCategory (ID_CONTEXT2);
	
		// retreive focus back to the view , becuse html editor stole it..
		GetActiveAvocadoView ()->SetFocus ();
		
	//	else
		//	m_wndWebView.SetCurrentHtml ("");
	}
void CMainFrame::SetDockingWindowIcons(BOOL bHiColorIcons)
{
	if (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP)
	{
		HICON hFileViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndFileView.SetIcon(hFileViewIcon, FALSE);
	}
	else
	{
		HICON hShellViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
		m_wndShellView.SetIcon(hShellViewIcon, FALSE);
	}

	HICON hWebViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_FILE_VIEW_HC : IDI_FILE_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndWebView.SetIcon(hWebViewIcon, FALSE);

	HICON hClassViewIcon = (HICON) ::LoadImage(::AfxGetResourceHandle(), MAKEINTRESOURCE(bHiColorIcons ? IDI_CLASS_VIEW_HC : IDI_CLASS_VIEW), IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 0);
	m_wndClassView.SetIcon(hClassViewIcon, FALSE);

}

BOOL CMainFrame::CreateOutlookBar(CMFCOutlookBar& bar, UINT uiID, CMFCShellTreeCtrl& tree, CWebBrowserBar& calendar, int nInitialWidth)
{
	m_outlookBarVisible = true;
	bar.SetMode2003();

	BOOL bNameValid;
	CString strTemp;
	bNameValid = strTemp.LoadString(IDS_SHORTCUTS);
	ASSERT(bNameValid);
	if (!bar.Create(strTemp, this, CRect(0, 0, nInitialWidth, 32000), uiID, WS_CHILD | WS_VISIBLE |CBRS_LEFT | CBRS_FLOATING | AFX_CBRS_AUTOHIDE   ))
	{
		return FALSE; // fail to create
	}

	CMFCOutlookBarTabCtrl* pOutlookBar = (CMFCOutlookBarTabCtrl*)bar.GetUnderlyingWindow();

	if (pOutlookBar == NULL)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	pOutlookBar->EnableInPlaceEdit(TRUE);

	static UINT uiPageID = 1;

	// can float, can autohide, can resize, CAN NOT CLOSE
	DWORD dwStyle = AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE | AFX_CBRS_RESIZE;

	CRect rectDummy(0, 0, 0, 0);
	const DWORD dwTreeStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	
	calendar.Create(rectDummy,&bar,1201);
	bNameValid = strTemp.LoadString(IDS_CALENDAR);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&calendar/*&calendar*/, _T("Galleries"), 2, TRUE, dwStyle);

	tree.Create(dwTreeStyle, rectDummy, &bar, 1202);
	bNameValid = strTemp.LoadString(IDS_FOLDERS);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&tree, strTemp, 3, TRUE, dwStyle);

	//calendar.Create(rectDummy, &bar, 1201);
	m_wndServerView.Create(rectDummy,&bar,1203);
	bNameValid = strTemp.LoadString(IDS_CALENDAR);
	ASSERT(bNameValid);
	pOutlookBar->AddControl(&m_wndServerView/*&calendar*/, strTemp, 3, TRUE, dwStyle);

	bar.SetPaneStyle(bar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC | AFX_CBRS_AUTOHIDE);
	bar.SetControlBarStyle (AFX_CBRS_AUTOHIDE);
	pOutlookBar->SetVisiblePageButtons (0);
	pOutlookBar->SetImageList(theApp.m_bHiColorIcons ? IDB_PAGES_HC : IDB_PAGES, 24);
	pOutlookBar->SetToolbarImageList(theApp.m_bHiColorIcons ? IDB_PAGES_SMALL_HC : IDB_PAGES_SMALL, 16);
	pOutlookBar->RecalcLayout();

	BOOL bAnimation = theApp.GetInt(_T("OutlookAnimation"), TRUE);
	CMFCOutlookBarTabCtrl::EnableAnimation(bAnimation);
	
	bar.SetButtonsFont(&afxGlobalData.fontBold);
	
	return TRUE;
}


BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return FALSE;
	}

	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_FILE_IMPORT2, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // force Print Preview mode closed
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

bool CMainFrame::IsActiveDocumentNonEmpty ()
{
	if (GetActiveDocument ())
	{
	bool hasElements = (((CAvocadoDoc*)GetActiveDocument())->GetElementsCount () > 0);
	return hasElements;
	}
	return false;
}

int CMainFrame::GetActiveViewIntParam (const char* param)
{
	// All view related update commands should be moved to view..
	int paramValue = 0;
	if (GetActiveView ())
	{
		 paramValue = (GetActiveAvocadoView()->GetViewParameterAsInt (param));
	}
	return paramValue;
}
CAvocadoView* CMainFrame::GetActiveAvocadoView ()
{
	if (GetActiveView())
		return ((CAvocadoView*)GetActiveView ());
	return false;
}
void CMainFrame::OnUpdateSelectPart(CCmdUI *pCmdUI)
{
//	bool hasElements = (((CAvocadoDoc*)GetActiveDocument())->GetElementsCount () > 0);
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
	
}


void CMainFrame::OnUpdateOrientationFitPage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationBack(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationFront(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationLeft(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationRight(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationTop(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationBottom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationCustom(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationDef1(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateOrientationDef2(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateSelectArea(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateSelectAll(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateStyleFlat(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewIntParam("RenderStyle") == RENDER_STYLE_FLAT);
	
		pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateStyleHlr(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewIntParam("RenderStyle")  == RENDER_STYLE_HLR);
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateStyleToon(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewIntParam("RenderStyle")  == RENDER_STYLE_TOON);
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateStyleShaded(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewIntParam("RenderStyle")  == RENDER_STYLE_SHADED);
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}

bool CMainFrame::IsActiveViewOrthographic ()
{
	int ortho = (GetActiveAvocadoView()->GetViewParameterAsInt ("OrthographicView"));
	return !!ortho;
}

void CMainFrame::OnUpdateStyleOrthographicView(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (IsActiveDocumentNonEmpty());
		pCmdUI->SetCheck (IsActiveViewOrthographic());
		//bool CAvocadoView::IsOrthographic ();
}
int CMainFrame::GetActiveDocID ()
{
	if (GetActiveAvocadoDoc())
		return 	GetActiveAvocadoDoc()->GetAvoID();
	return -1;
}
int CMainFrame::GetActiveViewID ()
{
	if (GetActiveAvocadoView())
		return 	GetActiveAvocadoView()->GetID();
	return -1;
}
void CMainFrame::OnStyleShaded()
{
//	((CAvocadoView*)GetActiveView())->SetRenderStyle (RENDER_STYLE_SHADED);
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string RenderStyle=1;");

}


void CMainFrame::OnStyleToon()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string RenderStyle=2;");

	//((CAvocadoView*)GetActiveView())->SetRenderStyle (RENDER_STYLE_TOON);
}


void CMainFrame::OnStyleHlr()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string RenderStyle=3;");

	//((CAvocadoView*)GetActiveView())->SetRenderStyle (RENDER_STYLE_HLR);
}


void CMainFrame::OnStyleFlat()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string RenderStyle=4;");

	//((CAvocadoView*)GetActiveView())->SetRenderStyle (RENDER_STYLE_FLAT);
}


void CMainFrame::OnUpdateMenusStyle(CCmdUI *pCmdUI)
{
	bool isUsingOptix = (GetActiveViewIntParam("UseOptix") == 1);

	pCmdUI->Enable (IsActiveDocumentNonEmpty() && (!isUsingOptix));
	//pCmdUI->m_nIndex
	
}


void CMainFrame::OnUpdateMenusSelect(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
	
}


void CMainFrame::OnUpdateMenusOrientaion(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnStyleOrthographicView()
{
	bool newVal = (!IsActiveViewOrthographic());
	string strVal = (newVal ? "1" : "0");
	GetActiveAvocadoView()->SetViewParameter ("OrthographicView",strVal );
}


void CMainFrame::OnUpdateAppreanceHide(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsCurrentSelectionVisible());
	
}

void CMainFrame::OnUpdateEditClear(CCmdUI *pCmdUI)
{
		pCmdUI->Enable(ActiveViewHasSelection ());

	
}


void CMainFrame::OnUpdateEditCopy(CCmdUI *pCmdUI)
{
		pCmdUI->Enable(ActiveViewHasSelection ());

	
}

bool CMainFrame::ActiveViewHasSingleSelection ()
{
	if (GetActiveAvocadoView())
	{
		return (GetActiveAvocadoView()->HasSelection () && GetActiveAvocadoView()->GetSelectionCount () == 1);
	}
	return false;
}
bool CMainFrame::ActiveViewHasSelection ()
{
	if (GetActiveAvocadoView())
	{
		return GetActiveAvocadoView()->HasSelection ();
	}
	return false;
}

void CMainFrame::OnUpdateEditCut(CCmdUI *pCmdUI)
{		
		pCmdUI->Enable(ActiveViewHasSelection ());	
}


void CMainFrame::OnUpdateOrientationLookAt(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());	
}


void CMainFrame::OnEditCopy()
{
	if (ActiveViewHasSelection ())
	{
		m_clipboard = GetActiveAvocadoView()->GetSelection ();
	}
}


void CMainFrame::OnEditClear()
{
	m_clipboard = -1;
	if (ActiveViewHasSelection())
	{
		if (AfxMessageBox (L"This will delete the selected element\n and all of its children. Continue with delete ?",MB_YESNO | MB_ICONSTOP)== IDYES)
		{
		int eid = GetActiveAvocadoView ()->GetSelection();
			string pr;
			std::stringstream pStr;
			pStr <<  "int eid=" << eid << ",bool unhighlight=1;";
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("DeleteDocCommonElement",GetActiveDocID (),pStr.str().c_str());	
		}
	}
	
}


void CMainFrame::OnEditCut()
{
	if (ActiveViewHasSelection ())
	{
		m_clipboard = GetActiveAvocadoView()->GetSelection ();
	}
}


void CMainFrame::OnUpdateEditPaste(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_clipboard != -1);
	
}


void CMainFrame::OnModelMove()
{
	ToggleViewFilter("DraggerFilters",MODEL_MOVE);
	
}


void CMainFrame::OnUpdateModelMove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_MOVE));
	
}


void CMainFrame::OnModelRotate()
{
	ToggleViewFilter("DraggerFilters",MODEL_ROTATE);
}


bool CMainFrame::GetActiveViewFilter (const char* param,unsigned int filt)
{
	if (GetActiveView ())
	{
	unsigned int df = GetActiveAvocadoView()->GetViewParameterAsInt(param);
	if ((df & filt) == filt)
		return true;
	}
	return false;
}


void CMainFrame::OnUpdateModelRotate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_ROTATE));
}


void CMainFrame::OnModelScale()
{
//	bool newVal = (!IsActiveViewOrthographic());
	ToggleViewFilter("DraggerFilters",MODEL_SCALE);
	
}

void CMainFrame::ToggleViewFilter (const char* param , unsigned int filt)
{
	unsigned int df = GetActiveAvocadoView()->GetViewParameterAsInt (param);

	df =  df ^ filt;
	std::stringstream idStr;
	idStr << df;
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),string (string("string ")+param+string ("=")+idStr.str()+string(";")).c_str());
	
	//GetActiveAvocadoView()->SetViewParameter (param,idStr );
}
void CMainFrame::OnUpdateModelScale(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_SCALE));
}


void CMainFrame::OnUpdateDisplayAnnotation(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewFilter("DisplayFilters",DISPLAY_ANNOTATIONS));
}


void CMainFrame::OnUpdateDisplayBackground(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewFilter("DisplayFilters",DISPLAY_BACKGROUND));

	
}


void CMainFrame::OnUpdateDisplayCameras(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewFilter("DisplayFilters",DISPLAY_CAMERAS));
}


void CMainFrame::OnUpdateDisplayLights(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewFilter("DisplayFilters",DISPLAY_LIGHTS));
}


void CMainFrame::OnUpdateDisplaySpincenter(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewFilter("DisplayFilters",DISPLAY_SPIN_CENTER));
}


void CMainFrame::OnDisplaySpincenter()
{
	ToggleViewFilter("DisplayFilters",DISPLAY_SPIN_CENTER);
}


void CMainFrame::OnDisplayLights()
{
	ToggleViewFilter("DisplayFilters",DISPLAY_LIGHTS);
}

void CMainFrame::OnDisplayCameras()
{
	ToggleViewFilter("DisplayFilters",DISPLAY_CAMERAS);
	
}


void CMainFrame::OnDisplayAnnotation()
{
	ToggleViewFilter("DisplayFilters",DISPLAY_ANNOTATIONS);
}


void CMainFrame::OnDisplayBackground()
{
	ToggleViewFilter("DisplayFilters",DISPLAY_BACKGROUND);
	
}


void CMainFrame::OnUpdateTrackingUsermode(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(GetActiveViewIntParam("ManipulationMode") == TRACK_MODE_USER);
}


void CMainFrame::OnUpdateTrackingFlymode(CCmdUI *pCmdUI)
{
		pCmdUI->SetCheck(GetActiveViewIntParam("ManipulationMode") == TRACK_MODE_FLY);
}


void CMainFrame::OnUpdateTrackingCadmode(CCmdUI *pCmdUI)
{
		pCmdUI->SetCheck(GetActiveViewIntParam("ManipulationMode") == TRACK_MODE_CAD);
}


void CMainFrame::OnTrackingCadmode()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string ManipulationMode=1;");
}


void CMainFrame::OnTrackingFlymode()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string ManipulationMode=3;");
}


void CMainFrame::OnTrackingUsermode()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string ManipulationMode=2;");
	
	
}


void CMainFrame::OnGraphicsRaytracing()
{
	if (GetActiveViewIntParam("UseOptix") == 1)
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string UseOptix=0;");
	else
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string UseOptix=1;");
}


void CMainFrame::OnUpdateGraphicsRaytracing(CCmdUI *pCmdUI)
{
	
	bool isOptixSupported = (GetActiveViewIntParam("supportOptix") == 1);
	if (isOptixSupported)
	{
	bool isUsingOptix = (GetActiveViewIntParam("UseOptix") == 1);
	//pCmdUI->m_nIndex = isUsingOptix;
	pCmdUI->SetCheck(isUsingOptix);
	}
	pCmdUI->Enable(isOptixSupported);
	
}


void CMainFrame::OnGraphicsAntiAliasing()
{
	if (GetActiveViewIntParam("AntiAliasing") == 1)
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string AntiAliasing=0;");
	else
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string AntiAliasing=1;");

	
}


void CMainFrame::OnUpdateGraphicsAntiAliasing(CCmdUI *pCmdUI)
{
	bool isAASupported = (GetActiveViewIntParam("supportAA") == 1);
	if (isAASupported)
	{
		bool isAA = (GetActiveViewIntParam("AntiAliasing") == 1);
		pCmdUI->m_nIndex = isAA;
		pCmdUI->SetCheck(isAA);
	}
	pCmdUI->Enable(isAASupported);
}


void CMainFrame::OnUpdateGraphicsStereo(CCmdUI *pCmdUI)
{
	bool isStereo = (GetActiveViewIntParam("StereoView") == 1);
	pCmdUI->m_nIndex = isStereo;
	pCmdUI->SetCheck(isStereo);
	pCmdUI->Enable(true);
}


void CMainFrame::OnGraphicsStereo()
{
	if (GetActiveViewIntParam("StereoView") == 1)
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string StereoView=0;");
	else
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string StereoView=1;");

}

bool CMainFrame::IsCurrentSelectionHidden()
{
	bool selectionIsVisible = false;
	if (GetActiveView())
	{
		int sel = GetActiveAvocadoView()->GetSelection ();
		if (sel > -1)
		{
				GetActiveAvocadoDoc ()->GetElementVisibility (sel,selectionIsVisible);
				if (selectionIsVisible == false)
					return true;
		}
	}
	return false;
}
bool CMainFrame::IsCurrentSelectionVisible()
{
	bool selectionIsVisible = false;
	if (GetActiveDocument () && GetActiveView())
	{
	int sel = GetActiveAvocadoView()->GetSelection ();
	if (sel > -1)
	{
			GetActiveAvocadoDoc ()->GetElementVisibility (sel,selectionIsVisible);
	}
 	}
	return (selectionIsVisible);	
}

bool CMainFrame::ActiveViewHasHiddenSelection()
{
	return (GetActiveDocument () && GetActiveView() && GetActiveAvocadoDoc ()->m_hasHiddenElements);
}
void CMainFrame::OnUpdateApperanceUnhide(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(IsCurrentSelectionHidden());
	
}


void CMainFrame::OnAppreanceHide()
{
	int selc = GetActiveAvocadoView()->GetSelectionCount ();
	if (selc > 0)
	{
		for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
		{
			int sel =  GetActiveAvocadoView()->GetSelection(selIDx);
			bool isVisible; 
			GetActiveAvocadoDoc ()->GetElementVisibility (sel,isVisible);
			if (isVisible)
			{
				int viewId = GetActiveAvocadoView()->GetID ();
				std::stringstream idStr;
				idStr << "int eid="<<sel <<",int vid=" <<viewId << ";";
				GetActiveAvocadoDoc()->AvocadoInvokeDoc("HideElement",GetActiveAvocadoDoc()->GetAvoID (),idStr.str ().c_str());
			}
		}
	}
	// TODO: Add your cnommand handler code here
}


void CMainFrame::OnAppreanceUnhideall()
{
	int viewId = GetActiveAvocadoView()->GetID ();
	std::stringstream idStr;
	idStr << "int vid=" <<viewId << ";";

	GetActiveAvocadoDoc()->AvocadoInvokeDoc("UnHideAllElements",GetActiveAvocadoDoc()->GetAvoID() ,idStr.str().c_str());
	
	
}


void CMainFrame::OnApperanceUnhide()
{
	int selc = GetActiveAvocadoView()->GetSelectionCount ();
	if (selc > -1)
	{
		for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
		{
			int sel =  GetActiveAvocadoView()->GetSelection(selIDx);
			bool isVisible; 
			GetActiveAvocadoDoc ()->GetElementVisibility (sel,isVisible);
			if (!isVisible)
			{
				int viewId = GetActiveAvocadoView()->GetID ();
				std::stringstream idStr;
				idStr << "int eid="<<sel <<",int vid=" <<viewId << ";";
				GetActiveAvocadoDoc()->AvocadoInvokeDoc("UnHideElement",GetActiveAvocadoDoc()->GetAvoID (),idStr.str ().c_str());
			}
		}
	}
	
}


//void CMainFrame::s(CCmdUI *pCmdUI)
//{
//	
//}

void CMainFrame::OnUpdateIndicator(CCmdUI *pCmdUI)
{
	const CAvocadoView * vi = GetActiveAvocadoView ();
	if (vi)
	{
	wchar_t dst[100];
	::wsprintfW (dst,L"(%d, %d)",vi->GetMouseX(),vi->GetMouseY());
	pCmdUI->SetText (dst);
	}
}
void CMainFrame::OnUpdateAppreanceUnhideall(CCmdUI *pCmdUI)
{
	
	pCmdUI->Enable (ActiveViewHasHiddenSelection());
	
}


void CMainFrame::OnOrientationBack()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Back");
	
}


void CMainFrame::OnOrientationBottom()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Bottom");
	
}


void CMainFrame::OnOrientationCustom()
{
	
}


void CMainFrame::OnOrientationDef1()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Def1");
}


void CMainFrame::OnOrientationDef2()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Def2");
}


void CMainFrame::OnOrientationFront()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Front");
}


void CMainFrame::OnOrientationTop()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Top");
}


void CMainFrame::OnOrientationLeft()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Left");
	
}


void CMainFrame::OnOrientationRight()
{
	 GetActiveAvocadoDoc()->AvocadoInvokeView("ViewCamera_Orient",GetActiveViewID (),"Right");
	
}


void CMainFrame::OnOrientationLookAt()
{
	if (GetActiveAvocadoView ())
	{
		int sel = GetActiveAvocadoView()->GetSelection ();
		int vid = GetActiveAvocadoView()->GetID ();

	    char dst[3];
	    itoa (sel,dst,10);
		char vids[3];
	    itoa (vid,vids,10);
		string pStr = "int eid="+string (dst) +",int vid="+ string (vids) + ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("LookAt",GetActiveDocID(),pStr.c_str());
	}
}

void CMainFrame::OnTakeSnapshot ()
{

	CString spath = theApp.GetAppSessionNextTempFileName ( CString (L".jpg" ));
	const wchar_t * sp = spath.GetBuffer ();
	char cp[200];
	::wcstombs (cp,sp,200);
	 GetActiveAvocadoDoc()->AvocadoInvokeView("SaveViewToFile",GetActiveViewID (),std::string (cp).c_str());
	m_wndWebView.EmbbedImage (spath,50,50);
	
}

void CMainFrame::UpdateViewStateGallery(int vsCount)
{
	CMFCToolBarImages pal;
	pal.SetImageSize  (CSize(256,256));
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_GOTOPAGEGALLERY,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;


	for (int i=0;i< vsCount;i++)
	{
		CString tempF =theApp.GetAppSessionTempFolder ();
		std::string imageName;
		imageName = GetActiveAvocadoDoc()->GetViewStateImage (i);
				char tempC [MAX_PATH];
				::wcstombs (tempC,tempF.GetString (),MAX_PATH);
				stringstream dst ;
				dst << string (tempC) << string ("\\") << imageName ;

	CString tfile ;//= CString(GetProccessDirectory().c_str());
				wchar_t filename[MAX_PATH];
				::mbstowcs (filename,dst.str ().c_str(),MAX_PATH);
				tfile =  CString (filename);
			
				HBITMAP hbmp = (HBITMAP)::LoadImage(NULL,tfile.GetString(),
				IMAGE_BITMAP,256,256,LR_LOADFROMFILE);	
			
				pal.AddImage (hbmp);	
		}

		galel->SetIconsInRow (4);
		if (pal.GetCount()>0)
			galel->SetPalette (pal);
		for (int i=0;i<pal.GetCount();i++)
		{
			stringstream dst ;
			dst << "ViewState " << i  ;

			galel->SetItemToolTip (int(i),CString(dst.str().c_str()));
		}

}
void CMainFrame::OnHtmlEditorSave ()
{
	std::string HtmlText ;
#ifdef _DEBUG 
	CMemoryState oldMemState, newMemState, diffMemState; 
	oldMemState.Checkpoint(); 
#endif
		HtmlText = m_wndWebView.GetCurrentHtml ();
#ifdef _DEBUG 
	newMemState.Checkpoint(); 
	if( diffMemState.Difference( oldMemState, newMemState ) ) 
	{ 
		TRACE( "Memory leaked!\n" ); 
		diffMemState.DumpStatistics(); 
	} 
#endif

	string pr;
	if (GetActiveAvocadoView ())
	{
		int sel = GetActiveAvocadoView()->GetSelection ();
		if (sel >= 0)
		{
			GetActiveAvocadoDoc ()->SetElementMetaData (sel,"Html",HtmlText);
		} else
		{
			int currentVS = GetActiveAvocadoDoc()->GetCurrentViewState ();
			GetActiveAvocadoDoc()->SetViewStateHtml (currentVS,HtmlText);
		
			if (theApp.GetAppMode () != CAvocadoApp::VIEWER_APP)
			{
				std::string imageName;
				imageName = GetActiveAvocadoDoc()->GetViewStateImage (currentVS);

				CString tempF =theApp.GetAppSessionTempFolder () + CString ("\\");
				char tempC [MAX_PATH];
				::wcstombs (tempC,tempF.GetString (),MAX_PATH);
				stringstream dst ;
				if (imageName == "")
				{
					stringstream newImageName ;
					newImageName << "avocadoViewState_" << currentVS << ".bmp" ;
					dst << string (tempC) << "avocadoViewState_" << currentVS << ".bmp" ;
					imageName = GetActiveAvocadoDoc()->SetViewStateImage(currentVS,newImageName.str());
				}
				else
				{
					dst << string (tempC) << imageName;
				}
				string fileParam = "string filename="+dst.str () + string (",int x=256,int y=256,bool forceOptix=0;");
				 GetActiveAvocadoDoc()->AvocadoInvokeView ("SaveViewToBitmapFile",GetActiveViewID (),fileParam.c_str());

			} 
		}
	//	GetActiveAvocadoView ()->SetFocus ();
	}
		return;
}
void CMainFrame::RefreshDocLoadStatus ()
{
	if (GetActiveDocument())
	{
		static int s_last_prog = 0;
				CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> elarr;
				CMFCRibbonStatusBarPane* el = (CMFCRibbonStatusBarPane*)(m_wndStatusBar.FindElement(ID_STATUSBAR_PANE3));
				CMFCRibbonProgressBar* el2 = (CMFCRibbonProgressBar*)(m_wndStatusBar.FindElement(ID_STATUSBAR_PROGRESS));
				
				if (el != NULL && el2 != NULL)
				{
					std::string strMsg = "";
					int prog = -1;
					GetActiveAvocadoDoc()->GetDocumentStatus(strMsg,prog);
					char progStr[20];
					itoa (prog,progStr,10);
					strMsg += " (" +string (progStr)+"%)";
					if (prog < 0)
						prog =0;
					if (prog > 100) 
						prog = 100;
					el->SetText (CString(strMsg.c_str()));
					el2->SetRange (0,100);
					if (abs(s_last_prog - prog) > 5)
					{
						s_last_prog = prog;
						GetActiveAvocadoView()->Invalidate ();
						//GetActiveAvocadoView()->UpdateWindow ();
						GetActiveAvocadoView()->RedrawWindow ();
					//	GetActiveAvocadoView()->InvokePaint ();
					}
					if (prog < 100 && prog > 0)
					{
						el2->SetPos (prog);
						el2->SetVisible(TRUE);
					} 
					 else
						el2->SetVisible(FALSE);
					
					m_wndStatusBar.RecalcLayout ();
					m_wndStatusBar.RedrawWindow();
				}
	}
}
void CMainFrame::RefreshViewStates ()
{
	
	UpdateViewStateGallery ( int(GetActiveAvocadoDoc()->GetViewStateCount ()));
	// why recalc layout here ??
//	this->m_wndRibbonBar.ForceRecalcLayout ();

	RefreshViewSelection();

}
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
#ifdef _DEBUG 
	CMemoryState oldMemState, newMemState, diffMemState; 
	oldMemState.Checkpoint(); 
#endif 
	// TODO: Add your specialized code here and/or call the base class
	NMHDR *pNMHDR  = (NMHDR*)lParam;
	if ( pNMHDR->code == AVC_REFRESH_DOC_LOAD_STATUS)
	{
		RefreshDocLoadStatus();
	} else if (pNMHDR->code == AVC_REFRESH_VIEW_STATES)
	{
		RefreshViewStates ();
	} else if (pNMHDR->code == AVC_REFRESH_MATERIAL_STATES)
	{
		RefreshMaterialStates ();
	} else if (pNMHDR->code == AVC_REFRESH_ACTIVE_DOCUMENT)
	{
		RefreshActiveDocument();
	} else
	if (pNMHDR->code == AVC_REFRESH_VIEW_SELECTION)
	{
		RefreshViewSelection();
	}
	else if (pNMHDR->code == TVN_ENDLABELEDIT)
	{
		OnRenameElement  ();
		return true;
	}
	if (pNMHDR->code == TVN_SELCHANGED)
	{
		OnClassViewSelection(pNMHDR,pResult);
		return true;
	}
	if (pNMHDR->code == ID_PROPERTIES)
	{
		OnTakeSnapshot ();
	} 
	else if (pNMHDR->code == ID_HTML_EDITOR_SAVE)
	{
		OnHtmlEditorSave ();
	} 
	else if (pNMHDR->code == ID_HTML_EDITOR_BACK)
	{
		OnLayoutBackpage();
	}
	else if (pNMHDR->code == ID_HTML_EDITOR_NEXT)
	{
		OnLayoutNextpage();
	}
#ifdef _DEBUG 
	newMemState.Checkpoint(); 
	if( diffMemState.Difference( oldMemState, newMemState ) ) 
	{ 
		TRACE( "Memory leaked!\n" ); 
		diffMemState.DumpStatistics(); 
	} 
#endif

	return CFrameWndEx::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnUpdateAvocadoviewMaterial(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);//ActiveViewHasSelection ());
	pCmdUI->SetCheck (m_waitCommand == ID_AVOCADOVIEW_MATERIAL);
	
}


void CMainFrame::OnEditPaste()
{
	if (m_clipboard != -1)
	
	{
	//	int eid = GetActiveAvocadoView ()->GetSelection();
			string pr;
			std::stringstream pStr;
			pStr <<  "int eid=" << m_clipboard << ";";
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("DuplicateDocFileElement",GetActiveDocID (),pStr.str().c_str());	
	}
	
}


void CMainFrame::OnSplitElement()
{
	int sel = GetActiveAvocadoView()->GetSelection ();
	char dst[3];
	itoa (sel,dst,10);

	std::string messa = std::string ("int eid=")+std::string (dst)+ string (",string geoName=new,int MetaCount=0,bool UpdateDocUI=1;");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddDocInstancedElement",GetActiveAvocadoDoc ()->GetAvoID (),messa.c_str());
}


void CMainFrame::OnUpdateSplitElement(CCmdUI *pCmdUI)
{
	// should really check for sub-selection also..
	pCmdUI->Enable(ActiveViewHasSingleSelection ());
}


void CMainFrame::OnTrackingPreselectionhightlights()
{
	if (GetActiveViewIntParam("PreSelectionEnabled") == 1)
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string PreSelectionEnabled=0;");
	else
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string PreSelectionEnabled=1;");

}


void CMainFrame::OnUpdateTrackingPreselectionhightlights(CCmdUI *pCmdUI)
{
	
	pCmdUI->SetCheck(GetActiveViewIntParam("PreSelectionEnabled") == 1);
}


void CMainFrame::SetMaterial(int id)
{

	int selID = GetActiveAvocadoView()->GetSelection ();
	if (selID > -1)
	{
		string pr;
		std::stringstream pStr;
		pStr <<  "int eid=" << selID
			<< ",int matid=" << id
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterial",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
		unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
		if (!(theApp.GetAppMode () == CAvocadoApp::VIEWER_APP))
		{
			m_wndRibbonBar.ShowContextCategories (ID_CONTEXT2,context != 0);
			m_wndRibbonBar.ActivateContextCategory (ID_CONTEXT2);
				m_wndRibbonBar.RecalcLayout ();
		}
	}
}

void CMainFrame::SetColorFromGallery (UINT cmdID)
{
	
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	this->m_wndRibbonBar.GetElementsByID (cmdID,arr);
	CMFCRibbonBaseElement *el = arr.GetAt (0);
	CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
	COLORREF cr = colel->GetColor();//GetHighlightedColor ();

	int r = GetRValue (cr);
	int g = GetGValue (cr);
	int b = GetBValue (cr);
	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
	std::stringstream pStr;
	pStr <<  "int eid=" << selID
		<< ",int red=" << r 
		<< ",int green=" << g
		<< ",int blue=" << b
		<< ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementColor",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	m_wndRibbonBar.ForceRecalcLayout();
}
void CMainFrame::OnMaterialColor()
{
	SetColorFromGallery (ID_MATERIAL_COLOR);
	
}


void CMainFrame::UpdateColorButton (CCmdUI *pCmdUI, UINT butID, bool butMode)
{
		bool isEnabled = false;
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	this->m_wndRibbonBar.GetElementsByID (butID,arr);
	CMFCRibbonBaseElement *el = arr.GetAt (0);
	CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
	COLORREF cr = RGB (255,0,0);
	if (butMode)
	{
	colel->SetButtonMode ((theApp.GetAppMode ()==CAvocadoApp::AUTHORING_APP));
	colel->SetColumns (7);
	}
	//colel->SetCompactMode (false);
	int selID = GetActiveAvocadoView()->GetSelection ();
	if (selID > -1)
	{
		std::vector <int> colv;
		GetActiveAvocadoDoc ()->GetElementColor (selID,colv);
		if (colv.size()>0)
		{
			if ((colv[0] != -1) && (colv[1]!=-1) && (colv[2] != -1))
					//colel->UpdateColor (RGB (colv[0],colv[1],colv[2]));
			{
				CList<COLORREF,COLORREF> cli;
				cli.AddHead  (RGB (colv[0],colv[1],colv[2]));
			
				colel->SetDocumentColors (L"Document",cli);//UpdateColor (RGB (colv[0],colv[1],colv[2]));
			}
			isEnabled = true;
		}
		else
		isEnabled = false;
	}
	
	pCmdUI->Enable (isEnabled);

}
void CMainFrame::OnUpdateMaterialColor(CCmdUI *pCmdUI)
{
	UpdateColorButton(pCmdUI,ID_MATERIAL_COLOR);
	
}


void CMainFrame::OnApperanceMaterial0()
{
	
	SetMaterial (0);
}


void CMainFrame::OnApperanceMaterial2()
{
	
	SetMaterial (2);
}


void CMainFrame::OnApperanceMaterial3()
{
	
	SetMaterial (3);
}


void CMainFrame::OnApperanceMaterial4()
{
	
	SetMaterial (4);
}


void CMainFrame::OnApperanceMaterial5()
{
	
	SetMaterial (5);
}


void CMainFrame::OnApperanceMaterial6()
{
	
	SetMaterial (6);
}


void CMainFrame::OnApperanceMaterial7()
{
SetMaterial (7);
}


void CMainFrame::OnApperanceMaterial1()
{
	SetMaterial (1);
}


void CMainFrame::OnOrientationFitPage()
{
	
GetActiveAvocadoDoc()->AvocadoInvokeDoc("FitToPage",GetActiveAvocadoDoc ()->GetAvoID (),"");
}


void CMainFrame::OnApperanceMaterial8()
{
	SetMaterial (8);
}


void CMainFrame::OnApperanceMaterial9()
{
	SetMaterial (9);
}

bool CMainFrame::ExportDocument (CString newName)
{
	wstring newNamew (newName);
	char dst[200];
	::wcstombs (dst,newNamew.c_str(),200);

	string newNameSt (dst);
	if (newNameSt.rfind (".") <= 0)
	{
		AfxMessageBox (_T("File name not valid"));
			return false;
	}
	string extension = newNameSt.substr (newNameSt.rfind ("."),newNameSt.size()-newNameSt.rfind (".")+1);
	if (extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".tif")
	{
		 GetActiveAvocadoDoc()->AvocadoInvokeView("SaveViewToFileFull",GetActiveViewID (),dst);
	} 
	else if (extension == ".bmp")
	{
		string fileParam = "string filename="+string (dst) + string (",int x=256,int y=256,bool forceOptix=1;");
		 GetActiveAvocadoDoc()->AvocadoInvokeView ("SaveViewToBitmapFile",GetActiveViewID (),fileParam.c_str());
	} 
	else if (extension == ".obj" || extension == ".nbf")
	{
			GetActiveAvocadoDoc()->AvocadoInvokeDoc ("ExportCurrentPage",GetActiveAvocadoDoc ()->GetAvoID (),dst);
	}

	return true;
}

void CMainFrame::OnModelResettodefault()
{
	int selID = GetActiveAvocadoView()->GetSelection ();
	if (selID != -1)
	{
		string pr;
		std::stringstream pStr;
		pStr <<  "int eid=" << selID
		<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("RestoreToDefault",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}


void CMainFrame::OnModelResettoori()
{
	int selID = GetActiveAvocadoView()->GetSelection ();
	if (selID != -1)
	{
		string pr;
		std::stringstream pStr;
		pStr <<  "int eid=" << selID

			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("RestoreToOrigin",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}


void CMainFrame::OnUpdateModelReset(CCmdUI *pCmdUI)
{
	
	pCmdUI->Enable(ActiveViewHasSelection ());
}


void CMainFrame::OnLayoutNewpage()
{
	
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ",bool fit=1;";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("NewPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}

}


void CMainFrame::OnLayoutNextpage()
{
	
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("NextPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}





void CMainFrame::OnLayoutBackpage()
{
	
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("BackPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}


void CMainFrame::OnLayoutDeletepage()
{
	
}


void CMainFrame::OnLayoutDuplicatepage()
{
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{

		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ",bool fit=0;";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("NewPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}

}


void CMainFrame::OnApperanceMaterial14()
{
	
	SetMaterial (14);
}


void CMainFrame::OnApperanceMaterial13()
{
	SetMaterial(13);
	
}


void CMainFrame::OnApperanceMaterial15()
{
	SetMaterial (15);
	
}


void CMainFrame::OnApperanceMaterial10()
{
	SetMaterial (10);
	
}


void CMainFrame::OnUpdateMaterialPlastic(CCmdUI *pCmdUI)
{
	
	pCmdUI->Enable (ActiveViewHasSelection());
}


void CMainFrame::OnUpdateMaterialWood(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (ActiveViewHasSelection());
}


void CMainFrame::OnUpdateMaterialStone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (ActiveViewHasSelection());
}


void CMainFrame::OnUpdateMaterialGlass(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (ActiveViewHasSelection());
}


void CMainFrame::OnUpdateMaterialFabric(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (ActiveViewHasSelection());
}


void CMainFrame::OnAvoacdogalleryFramegal()
{
	
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_FRAMEGAL,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
	int frameType  = galel->GetSelectedItem ();
	if (frameType == 0)
		OnLayoutFram ();
	else if (frameType == 1)
		this->OnLayoutFrametwoh();
	else if (frameType == 2)
		this->OnLayoutFramwtwow ();
	else if (frameType == 3)
			OnLayoutFramefour();
	galel->SetImageIndex (86 + frameType,true);
}


void CMainFrame::OnLayoutBackgroundcolor()
{
		
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_BACKGROUNDCOLOR,arr);
	CMFCRibbonBaseElement *el = arr.GetAt (0);
	CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
	COLORREF cr = colel->GetColor();//GetHighlightedColor ();

	string pr;
	std::stringstream pStr;
	pStr << int (cr); 
	string messa = string ("string backColor=")+pStr.str()+string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.c_str());
	m_wndRibbonBar.ForceRecalcLayout();

}


void CMainFrame::OnLayoutBackgroundgradient()
{
	
}

void CMainFrame::SetBackImagePreset (int preset)
{
	vector<string> filenames;
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\002.jpg");
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\003.jpg");
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\004.jpg");
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\005.jpg");
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\006.jpg");
	filenames.push_back ("C:\\Users\\Public\\Pictures\\PhotoStage\\007.jpg");

	string messa = string ("string backimage=") + filenames[preset] + string (";");
	//string extension = newNameSt.substr (newNameSt.rfind ("."),newNameSt.size()-newNameSt.rfind (".")+1);
	//if (extension == ".jpg" || extension == ".jpeg")
	{
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
	}

}
void CMainFrame::OnLayoutBackgroundimage()
{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_BACKGROUNDIMAGE,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
	int frameType  = galel->GetSelectedItem ();
	SetBackImagePreset (frameType);
}

void CMainFrame::DoLightsGallery (UINT cmd)
{
	
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (cmd,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
	int frameType  = galel->GetSelectedItem ();
	SetLightPreset (frameType);
}

void CMainFrame::OnAvoacdogalleryLights()
{
		DoLightsGallery(ID_AVOACDOGALLERY_LIGHTS);

}

void CMainFrame::SetLightPreset (int presetID)
{
	
	stringstream istr;
	istr << presetID;

	string messa = string ("string lightpreset=") + istr.str() + string (";");
	//string extension = newNameSt.substr (newNameSt.rfind ("."),newNameSt.size()-newNameSt.rfind (".")+1);
	//if (extension == ".jpg" || extension == ".jpeg")
	{
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
	}
}
void CMainFrame::OnLayoutFram()
{
		if (m_wndSplitter.GetRowCount () == 2)
		m_wndSplitter.DeleteRow (1);
		if (m_wndSplitter.GetColumnCount () == 2)
		m_wndSplitter.DeleteColumn (1);
}


void CMainFrame::OnLayoutFramwtwow()
{
	CCreateContext pCon;
	pCon.m_pCurrentDoc = GetActiveDocument ();
	pCon.m_pCurrentFrame = this;
	pCon.m_pNewDocTemplate = GetActiveDocument ()->GetDocTemplate ();
	pCon.m_pNewViewClass = RUNTIME_CLASS(CAvocadoView);
	// m_wndSplitter.CreateStatic( this, 2, 1);
	//m_wndSplitter.DeleteView (1,1);
	// m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
    //  &pCon);
		if (m_wndSplitter.GetRowCount () == 2)
		m_wndSplitter.DeleteRow (1);
if (m_wndSplitter.GetColumnCount () == 1)
{
	m_wndSplitter.SetColumnInfo (0,100,40);
	if (	m_wndSplitter.SplitColumn (100) )
		{	
				// m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
			//  &pCon);
	
			// m_wndSplitter.RecalcLayout();
		}
	}
}

void CMainFrame::OnLayoutFrametwoh()
{
	CCreateContext pCon;
	pCon.m_pCurrentDoc = GetActiveDocument ();
	pCon.m_pCurrentFrame = this;
	pCon.m_pNewDocTemplate = GetActiveDocument ()->GetDocTemplate ();
	pCon.m_pNewViewClass = RUNTIME_CLASS(CAvocadoView);
	// m_wndSplitter.CreateStatic( this, 2, 1);
	//m_wndSplitter.DeleteView (1,1);
	// m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
    //  &pCon);
	if (m_wndSplitter.GetColumnCount () == 2)
		m_wndSplitter.DeleteColumn (1);
	if (m_wndSplitter.GetRowCount () == 1)
	{
	m_wndSplitter.SetRowInfo (0,100,40);
	if (	m_wndSplitter.SplitRow (100) )
		{	
				// m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
			//  &pCon);
	
			// m_wndSplitter.RecalcLayout();
		}
	}
}

void CMainFrame::OnLayoutFramefour()
{
	CCreateContext pCon;
	pCon.m_pCurrentDoc = GetActiveDocument ();
	pCon.m_pCurrentFrame = this;
	pCon.m_pNewDocTemplate = GetActiveDocument ()->GetDocTemplate ();
	pCon.m_pNewViewClass = RUNTIME_CLASS(CAvocadoView);
	// m_wndSplitter.CreateStatic( this, 2, 1);
	//m_wndSplitter.DeleteView (1,1);
	// m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
    //  &pCon);
		RECT lpRect;
		GetClientRect (&lpRect);
	
	if (m_wndSplitter.GetColumnCount () ==1)
	{
		int colsize = 100;//int (float(lpRect.right-lpRect.left)/2.0f);
		colsize = abs (colsize);
		m_wndSplitter.SetColumnInfo (0,colsize,40);
		if (	m_wndSplitter.SplitColumn (colsize) )
			{	
			}
	}
	if (m_wndSplitter.GetRowCount () ==1)
	{
		int rowsize = 100;//int (float(lpRect.bottom-lpRect.top)/2.0f);
		rowsize = abs(rowsize);
		m_wndSplitter.SetRowInfo (0,rowsize,40);
			if (	m_wndSplitter.SplitRow (rowsize) )
			{
			}
				// m_wndSplitter.CreateView(0,1,RUNTIME_CLASS(CAvocadoView), CSize(100,100), 
			//  &pCon);
	
			// m_wndSplitter.RecalcLayout();
		}
	
}


void CMainFrame::OnUpdateLayoutFramefour(CCmdUI *pCmdUI)
{
	bool checked = ( m_wndSplitter.GetColumnCount () == 2 && 
		m_wndSplitter.GetRowCount () == 2 );
	pCmdUI->Enable ();
	pCmdUI->SetCheck (checked);

	
}


void CMainFrame::OnUpdateLayoutFrametwoh(CCmdUI *pCmdUI)
{
	
		bool checked = ( m_wndSplitter.GetColumnCount () == 1 && 
		m_wndSplitter.GetRowCount () == 2 );
	pCmdUI->Enable ();
	pCmdUI->SetCheck (checked);

}


void CMainFrame::OnUpdateLayoutFramwtwow(CCmdUI *pCmdUI)
{
		bool checked = ( m_wndSplitter.GetColumnCount () == 2 && 
		m_wndSplitter.GetRowCount () == 1 );
	pCmdUI->Enable ();
	pCmdUI->SetCheck (checked);

	
}


void CMainFrame::OnUpdateLayoutFram(CCmdUI *pCmdUI)
{
	bool checked = ( m_wndSplitter.GetColumnCount () == 1 && 
		m_wndSplitter.GetRowCount () == 1 );
	pCmdUI->Enable ();
	pCmdUI->SetCheck (checked);
	
}


void CMainFrame::OnLayoutBackgroundimageselect()
{
	
	CString filename = theApp.RaiseImageOpenDialog ();
	wstring newNamew (filename);

	char dst[200];
	::wcstombs (dst,newNamew.c_str(),200);

	string newNameSt (dst);
	string messa = string ("string backimage=") + newNameSt + string (";");
	//string extension = newNameSt.substr (newNameSt.rfind ("."),newNameSt.size()-newNameSt.rfind (".")+1);
	//if (extension == ".jpg" || extension == ".jpeg")
	{
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
	}
}


void CMainFrame::OnTexture()
{
		
	int sel = GetActiveAvocadoView ()->GetSelection ();
	if (sel > -1)
	{

		CString filename = theApp.RaiseImageOpenDialog ();
		wstring newNamew (filename);

		char dst[200];
		::wcstombs (dst,newNamew.c_str(),200);

		stringstream eidstr;
		eidstr << sel;

		string newNameSt (dst);

		string messa = string ("string image=") + newNameSt + string (",eid=") + eidstr.str() + string (";");
		//string extension = newNameSt.substr (newNameSt.rfind ("."),newNameSt.size()-newNameSt.rfind (".")+1);
		//if (extension == ".jpg" || extension == ".jpeg")
		{
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetElementTexture",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
		}
	}
}


void CMainFrame::OnBump()
{
		
	int sel = GetActiveAvocadoView ()->GetSelection ();
	if (sel > -1)
	{

		CString filename = theApp.RaiseImageOpenDialog ();
		wstring newNamew (filename);

		char dst[200];
		::wcstombs (dst,newNamew.c_str(),200);

		stringstream eidstr;
		eidstr << sel;

		string newNameSt (dst);

		string messa = string ("string image=") + newNameSt + string (",eid=") + eidstr.str() + string (";");
		{
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetElementBumpmap",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
		}
	}
}


void CMainFrame::OnLayoutSliderone()
{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_SLIDERONE,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
	float value = float( galel->GetPos () )/ float((galel->GetRangeMax() - galel->GetRangeMin()));
	{
		stringstream fval ;
		fval << value;
		string messa = string ("string globalLightAmbient=") + fval.str() + string (";");
		
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
	}
}

void CMainFrame::SetHeadLightOne (UINT cmd)
{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (cmd,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
	float value = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
	{
		stringstream fval ;
		fval << value;
		string messa = string ("string globalLightHeadlight=") + fval.str() + string (";");
		
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc()->GetAvoID(),messa.c_str());
	}
}
void CMainFrame::OnLayoutSlidertwo()
{
	
	SetHeadLightOne (ID_LAYOUT_SLIDERTWO);
}


void CMainFrame::OnUpdateLayoutNewpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty());
}


void CMainFrame::OnUpdateLayoutNextpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty() && 
		(GetActiveAvocadoDoc()->GetViewStateCount () > 1));
}


void CMainFrame::OnUpdateLayoutBackpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsActiveDocumentNonEmpty() && 
		(GetActiveAvocadoDoc()->GetViewStateCount () > 1));
}

void CMainFrame::UpdateHeadLightSlider (UINT cmd )
{
	if (GetActiveDocument ())
	{
				CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmd,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
	//	float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalLightHeadlight");
	//	if ( fabs(uivalue - docvalue)>0.1f)

			galel->SetPos (int (float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}
void CMainFrame::OnUpdateLayoutSlidertwo(CCmdUI *pCmdUI)
{
	UpdateHeadLightSlider(ID_LAYOUT_SLIDERTWO);
	
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateLayoutSliderone(CCmdUI *pCmdUI)
{
	if (GetActiveDocument ())
	{
			CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_SLIDERONE,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
	 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
	float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalLightAmbient");
	
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
	
	pCmdUI->Enable (true);
	
}


//void CMainFrame::OnFilePublish()
//{
//	theApp.
//}
void
CMainFrame::ExportDialog (int i)
{
	;
	CString newName;
	if (!theApp.DoPromptImportFileName(theApp.m_exportFilters,newName, AFX_IDS_SAVEFILECOPY,
	  OFN_HIDEREADONLY , FALSE, NULL,i))
		return; // open cancelled
		if (newName == CString (""))
		{
			AfxMessageBox (_T("File name not valid"));
			return;
		}
			
		ExportDocument (newName);
}
void
CMainFrame::ImportDialog (int i)
{
	CString newName;
	if (!theApp.DoPromptImportFileName(theApp.m_importFilters,newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL,i))
		return; // open cancelled

	AfxGetApp()->OpenDocumentFile(newName);
}

void CMainFrame::OnExportpageWavefront()
{
	ExportDialog (0);
}


void CMainFrame::OnExportpageJpeg()
{
	ExportDialog (2);
}


void CMainFrame::OnExportpageNvidia()
{
	ExportDialog (1);
}


void CMainFrame::OnExportpageBitmap()
{
	ExportDialog (3);
}


void CMainFrame::OnFileCollada()
{
	ImportDialog(0);
}


void CMainFrame::OnImport3dstudio()
{
	ImportDialog(1);
	
}


void CMainFrame::OnImportVrml()
{
	ImportDialog(2);
	
}


void CMainFrame::OnImportWavefront()
{
	
	ImportDialog(3);
}


void CMainFrame::OnImportNvidiaformat()
{
	ImportDialog(4);
	
}


//void CMainFrame::OnPublishToserver()
//{
//	
//}


//void CMainFrame::OnPublishTolocalarchive()
//{
//	
//}


void CMainFrame::OnAvocdomainTogglebrows()
{
	if (m_outlookBarVisible)
	{
		m_wndNavigationBar.ShowWindow (SW_HIDE);
		RecalcLayout ();
		UpdateWindow();

		m_outlookBarVisible = false;
	} else
	{
		m_wndNavigationBar.ShowWindow (SW_SHOW);
		RecalcLayout ();
		UpdateWindow();
		m_outlookBarVisible = true;
	}

	
}


void CMainFrame::OnUpdateFileUpdate(CCmdUI *pCmdUI)
{

}


void CMainFrame::OnApperanceMaterial11()
{
	SetMaterial(11);
}


void CMainFrame::OnApperanceMaterial12()
{
	SetMaterial(12);
}


void CMainFrame::OnApperanceMaterial16()
{
SetMaterial(16);
}


void CMainFrame::OnApperanceMaterial17()
{
	SetMaterial(17);
}


void CMainFrame::OnApperanceMaterial20()
{
	SetMaterial(20);
}


void CMainFrame::OnApperanceMaterial30()
{
	SetMaterial(30);
}

void CMainFrame::SetMaterialProp ( UINT uCmdID, string prop)
{
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (uCmdID,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	//::CMFCRibbonSpinButtonCtrl
	CMFCRibbonEdit *galel = ( CMFCRibbonEdit*)elb;
	CString uitext = galel->GetEditText ();

	// scale floats
	char c_val [10];
	::wcstombs (c_val,uitext.GetString(),10);
	float val = (float)atof(c_val);
	if (uCmdID ==  ID_MP_AMBIENT ||uCmdID == ID_MP_DIFFUSE || uCmdID == ID_MP_SPECULAR || 
		uCmdID  == ID_MP_OPACITY ||  uCmdID == ID_MP_ROUGHNESS || uCmdID  == ID_MP_POLISHNESS || 
		uCmdID  == ID_MP_MINREFL ||  uCmdID == ID_MP_MAXREFL || uCmdID == ID_MP_EXPOSURE  || uCmdID == ID_MP_REFLECTIVITY)
		val = val / 100.f;

	if (uCmdID == ID_MP_TEXTUREANGLE)
	{
		val = float(val * 2.0f * 3.14f ) / float(360.0f);
	}
	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
	std::stringstream pStr;

	pStr <<  "int eid=" << selID
		<< ",float propid=" << val
		<< ",string propname=" << prop
		<< ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialProp",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}

CMFCRibbonBaseElement * CMainFrame::GetRibbonBaseElement (UINT uCmdID)
{
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	m_wndRibbonBar.GetElementsByID (uCmdID,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	return elb;
}
int CMainFrame::GetRibbonSliderValue (UINT uCmdID)
{
	CMFCRibbonSlider *galel = ( CMFCRibbonSlider*)GetRibbonBaseElement(uCmdID);
	return (galel->GetPos ());
}
void CMainFrame::SetRibbonSliderValue (UINT uCmdID,int pos)
{
	CMFCRibbonSlider *galel = ( CMFCRibbonSlider*)GetRibbonBaseElement(uCmdID);
	 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
	if (galel->GetPos () != (int)pos)
			galel->SetPos( ((int)pos));
}

void CMainFrame::SetMaterialPropSlider ( UINT uCmdID, string prop)
{
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (uCmdID,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonSlider *galel = ( CMFCRibbonSlider*)elb;

	float val = (float)galel->GetPos ();
	if (uCmdID !=  ID_SLIDER_SHININESS &&
		uCmdID !=  ID_SLIDER_WOODSCALE1 && uCmdID !=  ID_SLIDER_RINGSCALE)
		val = val / 100.f;
	if (uCmdID == ID_SLIDER_TEXANGLE)
		// angle to radians..
		val = float(val*2.0f*3.14f) / float(360.0f);
	if (uCmdID == ID_SLIDER_HAIR_DENS)
		// angle to radians..
		val = val /10.0f;

	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
	std::stringstream pStr;

	pStr <<  "int eid=" << selID
		<< ",float propid=" << val
		<< ",string propname=" << prop
		<< ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialProp",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}


void CMainFrame::SetMaterialPropCheckBox ( UINT uCmdID, string prop)
{
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (uCmdID,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	//::CMFCRibbonSpinButtonCtrl
	//CMFCRibbonCheckBox *galel = ( CMFCRibbonCheckBox*)elb;
	CMFCRibbonBaseElement *galel = elb;
//	CString uitext = galel->GetPos ();

	// scale floats
	//char c_val [10];
	//::wcstombs (c_val,uitext.GetString(),10);
	float val = galel->IsChecked() ? 0.0f : 1.0f;//Pos ();//atof(c_val);
//	if (uCmdID !=  ID_SLIDER_SHININESS && uCmdID !=  ID_SLIDER_FRESNEL_EXP &&
	//	uCmdID !=  ID_SLIDER_WOODSCALE1 && uCmdID !=  ID_SLIDER_WOODSCALE1 && uCmdID !=  ID_SLIDER_RINGSCALE)
		/*||uCmdID == ID_MP_DIFFUSE || uCmdID == ID_MP_SPECULAR || 
		uCmdID  == ID_MP_OPACITY ||  uCmdID == ID_MP_ROUGHNESS || uCmdID  == ID_MP_POLISHNESS || 
		uCmdID  == ID_MP_MINREFL ||  uCmdID == ID_MP_MAXREFL || uCmdID == ID_MP_EXPOSURE)*/
	//	val = val / 100.f;

	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
	std::stringstream pStr;

	pStr <<  "int eid=" << selID
		<< ",float propid=" << val
		<< ",string propname=" << prop
		<< ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialProp",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}
void CMainFrame::OnMpDiffuse()
{
	SetMaterialProp ( ID_MP_DIFFUSE, "diffuse");
}

void CMainFrame::FetchMaterialValue (UINT cmdID)
{
	if (GetActiveDocument ())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
			if (context != ID_CONTEXT2)
				return;

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmdID,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		//::CMFCRibbonSpinButtonCtrl
		 CMFCRibbonEdit *galel = ( CMFCRibbonEdit*)elb;
		 if (galel->HasFocus ())
			 return;
		float uivalue = float(galel->GetData ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
		float docvalue = 0.0;
		if (ami)
		{
			   switch (cmdID)
			   {
			     case ID_MP_AMBIENT:
				    docvalue = ami->ambient * 100.0f;
					break;
				case ID_MP_DIFFUSE:
					docvalue = ami->diffuse * 100.0f;
					break;
				case ID_MP_SPECULAR:
					docvalue = ami->specular * 100.0f;
					break;
				case ID_MP_SHININESS:
					docvalue = ami->shininess;
					break;
				case ID_MP_OPACITY:
					docvalue = ami->opacity * 100.0f;
					break;
				case ID_MP_POLISHNESS:
					docvalue = ami->polished * 100.0f;
					break;
				case ID_MP_ROUGHNESS:
					docvalue = ami->roughness * 100.0f;
					break;
				case ID_MP_FRENSELEXP:
					docvalue = ami->frenselExp;
					break;
				case ID_MP_MINREFL:
					docvalue = ami->reflectionMin * 100.0f;
					break;
				case ID_MP_MAXREFL:
					docvalue = ami->reflectionMax * 100.0f;
					break;
				case ID_MP_EXPOSURE:
					docvalue = ami->exposure * 100.0f;
					break;
				case ID_MP_REFLECTIVITY:
					docvalue = ami->reflectivity * 100.0f;
					break;
				case ID_MP_TEXTUREANGLE:
					docvalue = ami->texAngle * 100.0f;
					docvalue = float(docvalue * 360.0f) / float(2.0f*3.41f);
					break;
				default:
					return;
			   }
		      
		}
		char dbb[5];
		itoa ((int)docvalue,dbb,10); 
		if (galel->GetEditText () != CString (dbb))
		galel->SetEditText(CString (dbb));
		}
//		if ( fabs(uivalue - docvalue)>0.1f)
		//	galel->SetText ( (int (float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}



void CMainFrame::FetchMaterialValueEditBox (UINT cmdID)
{
	//return;
	if (GetActiveDocument ())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
			if (context != ID_CONTEXT2)
				return;

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmdID,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		//::CMFCRibbonSpinButtonCtrl
		 CMFCRibbonEdit *galel = ( CMFCRibbonEdit*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
		float uivalue = float(galel->GetData ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
		string docvalue;
		if (ami)
		{
			   switch (cmdID)
			   {
			     case ID_EDITBOX_BUMPFILE:
					 docvalue = ami->bumpTexture.c_str();
					break;
				case ID_EDITBOX_TEXTUREFILE:
					 docvalue = ami->textureFilename.c_str();
					break;
					default:
					return;
			   }
		      
		}
		
		wchar_t docvalueUN [MAX_PATH];
		::mbstowcs (docvalueUN,docvalue.c_str (),MAX_PATH);
		if (CString (galel->GetText () )!= CString (docvalueUN))

			galel->SetEditText( docvalueUN );
		}
	}
}
void CMainFrame::FetchMaterialValueSlider (UINT cmdID)
{
	//This function has 2 versions for performance reasons.
	//return;
	if (GetActiveDocument ())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
			if (context != ID_CONTEXT2)
				return;
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmdID,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		//::CMFCRibbonSpinButtonCtrl
		 CMFCRibbonSlider *galel = ( CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
		float uivalue = float(galel->GetData ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
		float docvalue = 0.0;
		if (ami)
		{
			   switch (cmdID)
			   {
			     case ID_SLIDER_AMBIENT:
				    docvalue = ami->ambient * 100.0f;
					break;
				case ID_SLIDER:
					docvalue = ami->diffuse * 100.0f;
					break;
				case ID_SLIDER_SPECULAR2:
					docvalue = ami->specular * 100.0f;
					break;
				case ID_SLIDER_SHININESS:
					docvalue = ami->shininess;
					break;
				case ID_SLIDER_OPACITY:
					docvalue = ami->opacity * 100.0f;
					break;
				case ID_SLIDER_POLISHNESS:
					docvalue = ami->polished * 100.0f;
					break;
				case ID_SLIDER_ROUGHNESS:
					docvalue = ami->roughness * 100.0f;
					break;
				case ID_SLIDER_FRESNEL_EXP:
					docvalue = ami->frenselExp * 100.0f;
					break;
				case ID_SLIDER_MINREFL:
					docvalue = ami->reflectionMin * 100.0f;
					break;
				case ID_SLIDER_MAXREFL:
					docvalue = ami->reflectionMax * 100.0f;
					break;
				case ID_SLIDER_EXPOSURE:
					docvalue = ami->exposure * 100.0f;
					break;
				case ID_SLIDER_REFLECTIVITY:
					docvalue = ami->reflectivity * 100.0f;
					break;
				case ID_SLIDER_IOR:
					docvalue = ami->ior * 100.0f;
					break;
				
					/*
				case ID_SLIDER_BUMPHEIGHT:
					docvalue = ami->bumpHeight * 100.0f;
				break;
				case ID_SLIDER_BUMPSCALE:
					docvalue = ami->bumpScale * 100.0f;
				break;
			

				case ID_SLIDER_RINGSCALE:
					docvalue = ami->ringScale;// * 100.0f;
					break;
				case ID_SLIDER_WOODSCALE1:
					docvalue = ami->woodScale ;//* 100.0f;
					break;
				case ID_SLIDER_WOODSCALE2:
					docvalue = ami->wooble * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_WOODINTENSITY:
					docvalue = ami->woodColor[3] * 100.f;//* 100.0f;
					break;
				case  ID_SLIDER_SCALE_V:
					docvalue = ami->texScaleV * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_SCALEU:
					  docvalue = ami->texScaleU * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXOFFSETU:
					docvalue = ami->texOffsetU * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXOFFSETV:
					docvalue = ami->texOffsetV * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXANGLE:
					docvalue = ami->texAngle * 100.f;
					docvalue = float(docvalue * 360.0f) / float(2.0f*3.41f);
					break;
				case ID_SLIDER_HAIR_DENS:
					docvalue = ami->hairDens * 1000.f;
					break;
					
				case ID_SLIDER_HAIR_LENGTH:
					docvalue = ami->hairLengh * 100.f;
					break;
				case ID_SLIDER_SKIN_OIL:
					docvalue = ami->skinOil * 100.f;
					break;
				case ID_SLIDER_SKIN_SHIN:
					docvalue = ami->skinShin * 100.f;
					break;
					*/
				default:
					return;
			   }
		      
		}
		if (galel->GetPos () != (int)docvalue)
			galel->SetPos( ((int)docvalue));
		}
//		if ( fabs(uivalue - docvalue)>0.1f)
		//	galel->SetText ( (int (float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}
void CMainFrame::FetchMaterialValueSlider2 (UINT cmdID)
{
	//return;
	if (GetActiveDocument ())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
			if (context != ID_CONTEXT2)
				return;
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmdID,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		//::CMFCRibbonSpinButtonCtrl
		 CMFCRibbonSlider *galel = ( CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
			 return;
		float uivalue = float(galel->GetData ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
		float docvalue = 0.0;
		if (ami)
		{
			   switch (cmdID)
			   {
/*
			     case ID_SLIDER_AMBIENT:
				    docvalue = ami->ambient * 100.0f;
					break;
				case ID_SLIDER:
					docvalue = ami->diffuse * 100.0f;
					break;
				case ID_SLIDER_SPECULAR2:
					docvalue = ami->specular * 100.0f;
					break;
				case ID_SLIDER_SHININESS:
					docvalue = ami->shininess;
					break;
				case ID_SLIDER_OPACITY:
					docvalue = ami->opacity * 100.0f;
					break;
				case ID_SLIDER_POLISHNESS:
					docvalue = ami->polished * 100.0f;
					break;
				case ID_SLIDER_ROUGHNESS:
					docvalue = ami->roughness * 100.0f;
					break;
				case ID_SLIDER_FRESNEL_EXP:
					docvalue = ami->frenselExp * 100.0f;
					break;
				case ID_SLIDER_MINREFL:
					docvalue = ami->reflectionMin * 100.0f;
					break;
				case ID_SLIDER_MAXREFL:
					docvalue = ami->reflectionMax * 100.0f;
					break;
				case ID_SLIDER_EXPOSURE:
					docvalue = ami->exposure * 100.0f;
				break;
				case ID_SLIDER_REFLECTIVITY:
					docvalue = ami->reflectivity * 100.0f;
				break;
*/
				case ID_SLIDER_BUMPHEIGHT:
					docvalue = ami->bumpHeight * 100.0f;
				break;
				case ID_SLIDER_BUMPSCALE:
					docvalue = ami->bumpScale * 100.0f;
				break;


				case ID_SLIDER_RINGSCALE:
					docvalue = ami->ringScale;// * 100.0f;
					break;
				case ID_SLIDER_WOODSCALE1:
					docvalue = ami->woodScale ;//* 100.0f;
					break;
				case ID_SLIDER_WOODSCALE2:
					docvalue = ami->wooble * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_WOODINTENSITY:
					docvalue = ami->woodColor[3] * 100.f;//* 100.0f;
					break;
				case  ID_SLIDER_SCALE_V:
					docvalue = ami->texScaleV * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_SCALEU:
					  docvalue = ami->texScaleU * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXOFFSETU:
					docvalue = ami->texOffsetU * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXOFFSETV:
					docvalue = ami->texOffsetV * 100.f;//* 100.0f;
					break;
				case ID_SLIDER_TEXANGLE:
					docvalue = ami->texAngle * 100.f;
					docvalue = float(docvalue * 360.0f) / float(2.0f*3.41f);
					break;
				case ID_SLIDER_HAIR_DENS:
					docvalue = ami->hairDens * 1000.f;
					break;
					
				case ID_SLIDER_HAIR_LENGTH:
					docvalue = ami->hairLengh * 100.f;
					break;
				case ID_SLIDER_SKIN_OIL:
					docvalue = ami->skinOil * 100.f;
					break;
				case ID_SLIDER_SKIN_SHIN:
					docvalue = ami->skinShin * 100.f;
					break;

				default:
					return;
			   }
		      
		}
		if (galel->GetPos () != (int)docvalue)
			galel->SetPos( ((int)docvalue));
		}
//		if ( fabs(uivalue - docvalue)>0.1f)
		//	galel->SetText ( (int (float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}
bool CMainFrame::FetchMaterialValueCheckbox (UINT cmdID,CCmdUI *pCmdUI)
{
	//return false;
	if (GetActiveDocument ())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			unsigned int context = GetActiveAvocadoDoc ()->GetElementContext(selID);
			if (context != ID_CONTEXT2)
				return false;
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmdID,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		//::CMFCRibbonSpinButtonCtrle
		 //CMFCRibbonCheckBox *galel = ( CMFCRibbonCheckBox*)elb;
		CMFCRibbonBaseElement *galel = elb;//( CMFCRibbonCheckBox*)elb;
		// if (galel->IsPressed() || galel->IsFocused()  || galel->HasFocus ())
		//	 return false;
		//float uivalue = float(galel->GetData ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
		bool docvalue = false;
		if (ami)
		{
			   switch (cmdID)
			   {
			     case ID_MP_ISWOOD:
					 docvalue = ami->isWood;
					break;
				 case ID_CHECKBOX_BUMPNORMAL:
					 docvalue = ami->bumpIsNormalMap;
					 break;
				case ID_MP_ENABLEBUMP:
					 docvalue = ami->bumpEnabled;
					break;
					case ID_MP_ENABLETEXTURE:
						docvalue = ami->texEnabled;
					break;
				 case ID_MP_ENABLEHAIR:
					 docvalue = ami->isHair;
					break;
				case ID_MP_ENABLESKIN:
					 docvalue = ami->isSkin;
					 break;

				default:
					return 0;
			   }
		      
		}
	//	if (galel->IsChecked() != (int)docvalue)
			pCmdUI->SetCheck (docvalue);
		}
//		if ( fabs(uivalue - docvalue)>0.1f)
		//	galel->SetText ( (int (float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
	return false;
}
void CMainFrame::OnUpdateMpDiffuse(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_DIFFUSE);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpExposure(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_EXPOSURE);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpAmbient(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_AMBIENT);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpFrenselexp(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_FRENSELEXP);
	pCmdUI->Enable (true);
	
}


void CMainFrame::OnUpdateMpMaxrefl(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_MAXREFL);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpMinrefl(CCmdUI *pCmdUI)
{
   FetchMaterialValue(ID_MP_MINREFL);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpOpacity(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_OPACITY);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpPolishness(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_POLISHNESS);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpRoughness(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_ROUGHNESS);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpShininess(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_SHININESS);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpSpecular(CCmdUI *pCmdUI)
{
   FetchMaterialValue(ID_MP_SPECULAR);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateMpBasecolor(CCmdUI *pCmdUI)
{
	
	UpdateColorButton(pCmdUI,ID_MP_BASECOLOR,false);
}


void CMainFrame::OnMpBasecolor()
{
	SetColorFromGallery (ID_MP_BASECOLOR);
	
}


void CMainFrame::OnMpAmbient()
{
SetMaterialProp ( ID_MP_AMBIENT, "ambient");
}


void CMainFrame::OnMpExposure()
{
		SetMaterialProp ( ID_MP_EXPOSURE, "exposure");
}


void CMainFrame::OnMpFrenselexp()
{
		SetMaterialProp ( ID_MP_FRENSELEXP, "fresnel");
}


void CMainFrame::OnMpShininess()
{
	SetMaterialProp ( ID_MP_SHININESS, "shininess");
}


void CMainFrame::OnMpOpacity()
{
	SetMaterialProp ( ID_MP_OPACITY, "opacity");
}


void CMainFrame::OnMpRoughness()
{
	SetMaterialProp ( ID_MP_ROUGHNESS, "roughness");
}


void CMainFrame::OnMpSpecular()
{
		SetMaterialProp ( ID_MP_SPECULAR, "specular");
}


void CMainFrame::OnMpMinrefl()
{
	SetMaterialProp ( ID_MP_MINREFL, "minrefl");
}


void CMainFrame::OnMpMaxrefl()
{
	SetMaterialProp ( ID_MP_MAXREFL, "maxrefl");
}


void CMainFrame::OnMpPolishness()
{
	SetMaterialProp ( ID_MP_POLISHNESS, "polishness");
}


void CMainFrame::OnSlider()
{
	SetMaterialPropSlider ( ID_SLIDER, "diffuse");
}


void CMainFrame::OnSliderRoughness()
{
		SetMaterialPropSlider ( ID_SLIDER_ROUGHNESS, "roughness");
}


void CMainFrame::OnSliderSpecular()
{
//	SetMaterialPropSlider ( ID_SLIDER_SPECULAR, "specular");
}


void CMainFrame::OnSliderAmbient()
{
		SetMaterialPropSlider ( ID_SLIDER_AMBIENT, "ambient");
}


void CMainFrame::OnSliderOpacity()
{
		SetMaterialPropSlider ( ID_SLIDER_OPACITY, "opacity");
}


void CMainFrame::OnSliderShininess()
{
		SetMaterialPropSlider ( ID_SLIDER_SHININESS, "shininess");
}


void CMainFrame::OnUpdateSliderAmbient(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider(ID_SLIDER_AMBIENT);
	 pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSlider(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider(ID_SLIDER);
	 pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderSpecular(CCmdUI *pCmdUI)
{
	 //FetchMaterialValueSlider(ID_SLIDER_SPECULAR);
	 //pCmdUI->Enable (true); 
}


void CMainFrame::OnUpdateSliderOpacity(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_OPACITY);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderShininess(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_SHININESS);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderRoughness(CCmdUI *pCmdUI)
{
FetchMaterialValueSlider(ID_SLIDER_ROUGHNESS);
pCmdUI->Enable (true);
}


void CMainFrame::OnSliderReflectivity()
{
		SetMaterialPropSlider ( ID_SLIDER_REFLECTIVITY, "reflectivity");
}


void CMainFrame::OnUpdateSliderReflectivity(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_REFLECTIVITY);
	pCmdUI->Enable (true);
}


void CMainFrame::OnMpReflectivity()
{
	SetMaterialProp ( ID_MP_REFLECTIVITY, "reflectivity");
}


void CMainFrame::OnUpdateMpReflectivity(CCmdUI *pCmdUI)
{
	  FetchMaterialValue(ID_MP_REFLECTIVITY);
	   pCmdUI->Enable (true);
}


void CMainFrame::OnSliderMinrefl()
{
	SetMaterialPropSlider ( ID_SLIDER_MINREFL, "minrefl");
}


void CMainFrame::OnSliderMaxrefl()
{
	SetMaterialPropSlider ( ID_SLIDER_MAXREFL, "maxrefl");
}


void CMainFrame::OnUpdateSliderMaxrefl(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_MAXREFL);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderMinrefl(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_MINREFL);
	pCmdUI->Enable (true);
}


void CMainFrame::OnSliderFresnelExp()
{
    SetMaterialPropSlider ( ID_SLIDER_FRESNEL_EXP, "fresnel");
}


void CMainFrame::OnUpdateSliderFresnelExp(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_FRESNEL_EXP);
	pCmdUI->Enable (true);
}


void CMainFrame::OnSliderExposure()
{
	SetMaterialPropSlider ( ID_SLIDER_EXPOSURE, "exposure");
}


void CMainFrame::OnUpdateSliderExposure(CCmdUI *pCmdUI)
{
FetchMaterialValueSlider(ID_SLIDER_EXPOSURE);
	pCmdUI->Enable (true);
}


void CMainFrame::OnSliderPolishness()
{
	SetMaterialPropSlider ( ID_SLIDER_POLISHNESS, "polishness");
}


void CMainFrame::OnUpdateSliderPolishness(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_POLISHNESS);
	pCmdUI->Enable (true);
}


void CMainFrame::OnCheckboxWood()
{
//	SetMaterialPropCheckBox( ID_CHECKBOX_WOOD, "iswood");
}


void CMainFrame::OnUpdateCheckboxWood(CCmdUI *pCmdUI)
{

}


void CMainFrame::OnSliderRingscale()
{
	SetMaterialPropSlider ( ID_SLIDER_RINGSCALE, "ringscale");
}


void CMainFrame::OnUpdateSliderRingscale(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider2(ID_SLIDER_RINGSCALE);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnSliderWoodscale1()
{
SetMaterialPropSlider ( ID_SLIDER_WOODSCALE1, "woodscale");
}


void CMainFrame::OnUpdateSliderWoodscale1(CCmdUI *pCmdUI)
{
FetchMaterialValueSlider2(ID_SLIDER_WOODSCALE1);
	pCmdUI->Enable (true);// FetchMaterialValueCheckbox(ID_CHECKBOX_WOOD));
}


void CMainFrame::OnSliderWoodscale2()
{
	SetMaterialPropSlider ( ID_SLIDER_WOODSCALE2, "wooble");
}


void CMainFrame::OnUpdateSliderWoodscale2(CCmdUI *pCmdUI)
{
    FetchMaterialValueSlider2(ID_SLIDER_WOODSCALE2);
	pCmdUI->Enable ( true);//FetchMaterialValueCheckbox(ID_CHECKBOX_WOOD));
}


void CMainFrame::OnMpIswood()
{
	SetMaterialPropCheckBox( ID_MP_ISWOOD, "iswood");
}


void CMainFrame::OnUpdateMpIswood(CCmdUI *pCmdUI)
{
	FetchMaterialValueCheckbox(ID_MP_ISWOOD,pCmdUI) ;
}


void CMainFrame::OnSliderBumpheight()
{
	SetMaterialPropSlider ( ID_SLIDER_BUMPHEIGHT, "bumpheight");
}


void CMainFrame::OnSliderBumpscale()
{
	SetMaterialPropSlider ( ID_SLIDER_BUMPSCALE, "bumpscale");
}


void CMainFrame::OnUpdateSliderBumpheight(CCmdUI *pCmdUI)
{
	  FetchMaterialValueSlider2(ID_SLIDER_BUMPHEIGHT);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateSliderBumpscale(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_BUMPSCALE);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateEditboxBumpfile(CCmdUI *pCmdUI)
{
	FetchMaterialValueEditBox(ID_EDITBOX_BUMPFILE);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnStatic()
{
	
}


void CMainFrame::OnBrowseBump()
{
	CString image = theApp.RaiseImageOpenDialog ();
	

	if (image != CString (""))
	{
		char mif [MAX_PATH];
		::wcstombs (mif,image.GetString (),MAX_PATH);
		int selID = GetActiveAvocadoView()->GetSelection ();
		string pr;
		std::stringstream pStr;

		pStr <<  "int eid=" << selID
			<< ",string propid=" << mif
			<< ",string propname=bumptexture"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

	}
}




void CMainFrame::OnCheckboxBumpnormal()
{
	SetMaterialPropCheckBox( ID_CHECKBOX_BUMPNORMAL, "isnormal");
}


void CMainFrame::OnUpdateCheckboxBumpnormal(CCmdUI *pCmdUI)
{
	FetchMaterialValueCheckbox(ID_CHECKBOX_BUMPNORMAL,pCmdUI) ;
}




void CMainFrame::OnAvoacdogalleryBump()
{
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOACDOGALLERY_BUMP,data);
	
		string pr;
		std::stringstream pStr;
		int selID = GetActiveAvocadoView()->GetSelection ();
		pStr <<  "int eid=" << selID
			<< ",string propid="  << data 
			<< ",string propname=bumptexture"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}


void CMainFrame::OnAvoacdogalleryWoodcolor()
{
	if (GetActiveDocument())
	{
		    CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
			this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_WOODCOLOR,arr);
			CMFCRibbonBaseElement *el = arr.GetAt (0);
			CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
			COLORREF ref = colel->GetColor();
			float r = float (GetRValue (ref)) /255.0f;
			float g = float (GetGValue (ref)) /255.0f;
			float b = float (GetBValue (ref)) /255.0f;
			int selID = GetActiveAvocadoView()->GetSelection ();
			if (selID > -1)
			{
					std::stringstream pStr;
				pStr <<  "int eid=" << selID
				<< ",float f0=" << r
				<< ",float f1=" << g
				<< ",float f2=" << b
				<< ",string propname=woodcolor"
				<< ";";
				 GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialProp3Float",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
			}
	}
}



void CMainFrame::OnAvoacdogalleryWoodbump()
{
	/*CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
	this->m_wndRibbonBar.GetElementsByID (ID_AVOACDOGALLERY_WOODBUMP,arr2);
	CMFCRibbonBaseElement *elb = arr2.GetAt (0);
	CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
	int frameType  = galel->GetSelectedItem ();*/
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOACDOGALLERY_WOODBUMP,data);
	
	/*if (frameType > 6)
		frameType = 6;
		string pr;*/
		std::stringstream pStr;
		int selID = GetActiveAvocadoView()->GetSelection ();
		pStr <<  "int eid=" << selID
			<< ",string propid="<< data  
			<< ",string propname=bumptexture"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}


void CMainFrame::OnMpEnablebump()
{
	SetMaterialPropCheckBox( ID_MP_ENABLEBUMP, "enablebump");
}
void CMainFrame::OnUpdateMpEnablebump(CCmdUI *pCmdUI)
{
	FetchMaterialValueCheckbox(ID_MP_ENABLEBUMP,pCmdUI) ;
}
void CMainFrame::OnMpEnabletexture()
{
	SetMaterialPropCheckBox( ID_MP_ENABLETEXTURE, "enabletexture");
}


void CMainFrame::OnUpdateMpEnabletexture(CCmdUI *pCmdUI)
{
	FetchMaterialValueCheckbox(ID_MP_ENABLETEXTURE,pCmdUI) ;
}


void CMainFrame::OnMpClearbump()
{
	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
		std::stringstream pStr;

pStr <<  "int eid=" << selID
			<< ",string propid=empty" 
			<< ",string propname=bumptexture"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

}


void CMainFrame::OnSliderWoodintensity()
{
	SetMaterialPropSlider ( ID_SLIDER_WOODINTENSITY, "woodintensity");
}


void CMainFrame::OnUpdateSliderWoodintensity(CCmdUI *pCmdUI)
{
 FetchMaterialValueSlider2(ID_SLIDER_WOODINTENSITY);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnMpBrowsetexture()
{
	CString image = theApp.RaiseImageOpenDialog ();

	if (image != CString (""))
	{
		char mif [MAX_PATH];
		::wcstombs (mif,image.GetString (),MAX_PATH);
		int selID = GetActiveAvocadoView()->GetSelection ();
		string pr;
		std::stringstream pStr;

		pStr <<  "int eid=" << selID
			<< ",string propid=" << mif
			<< ",string propname=texturefilename"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

	}
}


void CMainFrame::OnAvoacdogalleryTexture()
{
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOACDOGALLERY_TEXTURE,data);
	
		std::stringstream pStr;
		int selID = GetActiveAvocadoView()->GetSelection ();
		pStr <<  "int eid=" << selID
			<< ",string propid="<< data 
			<< ",string propname=texturefilename"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}


void CMainFrame::OnSliderScaleu()
{
	SetMaterialPropSlider ( ID_SLIDER_SCALEU, "texturescaleu");
}


void CMainFrame::OnSliderScaleV()
{
	SetMaterialPropSlider ( ID_SLIDER_SCALE_V, "texturescalev");
}


void CMainFrame::OnUpdateEditboxTexturefile(CCmdUI *pCmdUI)
{
	FetchMaterialValueEditBox(ID_EDITBOX_TEXTUREFILE);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateSliderScaleV(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_SCALE_V);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateSliderScaleu(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_SCALEU);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnMpCleartexture()
{
int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
		std::stringstream pStr;

pStr <<  "int eid=" << selID
			<< ",string propid=empty" 
			<< ",string propname=texturefilename"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
}


void CMainFrame::OnSliderTexoffsetu()
{
	SetMaterialPropSlider ( ID_SLIDER_TEXOFFSETU, "textureoffsetu");
}


void CMainFrame::OnSliderTexoffsetv()
{
	SetMaterialPropSlider ( ID_SLIDER_TEXOFFSETV, "textureoffsetv");
}


void CMainFrame::OnUpdateSliderTexoffsetv(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_TEXOFFSETV);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateSliderTexoffsetu(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_TEXOFFSETU);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnSliderTexangle()
{
	SetMaterialPropSlider ( ID_SLIDER_TEXANGLE, "textureangle");
}


void CMainFrame::OnUpdateSliderTexangle(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_TEXANGLE);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnMpTextureangle()
{
	SetMaterialProp ( ID_MP_TEXTUREANGLE, "textureangle");
}


void CMainFrame::OnUpdateMpTextureangle(CCmdUI *pCmdUI)
{
	FetchMaterialValue(ID_MP_TEXTUREANGLE);
	pCmdUI->Enable (true);
}


void CMainFrame::OnMpEnableskin()
{
	SetMaterialPropCheckBox( ID_MP_ENABLESKIN, "enableskin");
}


void CMainFrame::OnMpEnablehair()
{
	SetMaterialPropCheckBox( ID_MP_ENABLEHAIR, "enablehair");
}


void CMainFrame::OnUpdateMpEnableskin(CCmdUI *pCmdUI)
{
	FetchMaterialValueCheckbox(ID_MP_ENABLESKIN,pCmdUI) ;
}


void CMainFrame::OnUpdateMpEnablehair(CCmdUI *pCmdUI)
{
FetchMaterialValueCheckbox(ID_MP_ENABLEHAIR,pCmdUI) ;
}


void CMainFrame::OnSliderHairLength()
{
	SetMaterialPropSlider ( ID_SLIDER_HAIR_LENGTH, "hairlength");
}


void CMainFrame::OnUpdateSliderHairLength(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_HAIR_LENGTH);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnSliderHairDens()
{
	SetMaterialPropSlider ( ID_SLIDER_HAIR_DENS, "hairdensity");
}


void CMainFrame::OnUpdateSliderHairDens(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_HAIR_DENS);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnAvoacdogalleryHair()
{
		string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOACDOGALLERY_HAIR,data);
	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
		std::stringstream pStr;

     pStr <<  "int eid=" << selID
			<< ",string propid=" << data
			<< ",string propname=hairtexturefilename"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

}


void CMainFrame::OnSliderSkinShin()
{
	SetMaterialPropSlider ( ID_SLIDER_SKIN_SHIN, "skinshin");
}


void CMainFrame::OnUpdateSliderSkinShin(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_SKIN_SHIN);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnUpdateSliderSkinOil(CCmdUI *pCmdUI)
{
	 FetchMaterialValueSlider2(ID_SLIDER_SKIN_OIL);
	pCmdUI->Enable ( true);
}


void CMainFrame::OnSliderSkinOil()
{
	SetMaterialPropSlider ( ID_SLIDER_SKIN_OIL, "skinoil");
}






void CMainFrame::OnSliderHeadlightL1()
{
	SetHeadLightOne (ID_SLIDER_HEADLIGHT_L1);
}


void CMainFrame::OnSliderHeadlightL2()
{
	SetHeadLightOne (ID_SLIDER_HEADLIGHT_L2);
}


void CMainFrame::OnSliderHeadlightL3()
{
	SetHeadLightOne (ID_SLIDER_HEADLIGHT_L3);
}


void CMainFrame::OnUpdateSliderHeadlightL1(CCmdUI *pCmdUI)
{
		UpdateHeadLightSlider (ID_SLIDER_HEADLIGHT_L1);
}


void CMainFrame::OnUpdateSliderHeadlightL2(CCmdUI *pCmdUI)
{
		UpdateHeadLightSlider (ID_SLIDER_HEADLIGHT_L2);

}
void CMainFrame::OnUpdateSliderHeadlightL3(CCmdUI *pCmdUI)
{
		UpdateHeadLightSlider (ID_SLIDER_HEADLIGHT_L3);
}


void CMainFrame::OnAvoacdogalleryLight3()
{
	DoLightsGallery(ID_AVOACDOGALLERY_LIGHT3);
}


void CMainFrame::OnAvoacdogalleryLight2()
{
	DoLightsGallery(ID_AVOACDOGALLERY_LIGHT2);
}


void CMainFrame::OnAvoacdogalleryLight1()
{
	DoLightsGallery(ID_AVOACDOGALLERY_LIGHT1);
}


void CMainFrame::OnMpSavePreset()
{
	string materialData;
	if (GetActiveDocument())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
			materialData = ami->ToString ();
			// we want to validate this string before saving it to file.
			//avocado::AvocadoMaterialInterface validationMaterial ;
			//validationMaterial.FromString (materialData);

			theApp.m_galleryManager.AddNewPreset (ID_AVOCADOVIEW_MATERIAL,materialData,"");
		}
	}
}

void CMainFrame::SetElementsMaterial (vector<int> selIDs,string data)
{
			std::stringstream pStr;
			if (selIDs.size()>0)
			{
				pStr << data ;
				for (int k=0;k<int(selIDs.size());k++)
					pStr << "int eid=" << selIDs[k] << "," ;

				GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropAll",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
			}
}
void CMainFrame::OnAvocadoviewMaterial()
{
	if (m_waitCommand == ID_AVOCADOVIEW_MATERIAL)
	{
		m_waitCommand = -1;
		return;
	}
	
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOCADOVIEW_MATERIAL,data);
	if (GetActiveDocument())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			m_waitCommand = -1;
			vector<int> selList;
			for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
				selList.push_back (GetActiveAvocadoView()->GetSelection (selIDx));
				SetElementsMaterial(selList,data);
		} else
		{
		    m_waitData = data;
			m_waitCommand = ID_AVOCADOVIEW_MATERIAL;
		}
	} 
}


BOOL CMainFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: Add your message handler code here and/or call default
	if ( (m_waitCommand > -1))
	{
		HCURSOR hCurs1 = LoadCursor(NULL, IDC_CROSS) ;
		SetCursor (hCurs1);
		return TRUE;
	} 
	
	return CFrameWndEx::OnSetCursor(pWnd, nHitTest, message);
}



void CMainFrame::OnEditPresetMaterial()
{
	if (theApp.m_galleryManager.EditPresets (ID_AVOCADOVIEW_MATERIAL))
		m_wndRibbonBar.ForceRecalcLayout ();

}


void CMainFrame::OnAvoacdogalleryEditbumppreset()
{
	if (theApp.m_galleryManager.EditPresets (ID_AVOACDOGALLERY_BUMP))
				m_wndRibbonBar.ForceRecalcLayout ();
	
}


void CMainFrame::OnAvoacdogalleryAddbumpgallery()
{
	string materialData;
	if (GetActiveDocument())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
			if (ami->bumpTexture != string ("") && ami->bumpTexture != string ("empty"))
			{
				if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_BUMP,string(ami->bumpTexture),string(ami->bumpTexture)))
					m_wndRibbonBar.ForceRecalcLayout ();
			} else
			{
				::AfxMessageBox (_T("No current bump map applied."));
			}
		}
	}
}


void CMainFrame::OnAvoacdogalleryAddtexturegallery()
{
	string materialData;
	if (GetActiveDocument())
	{
		int selID = GetActiveAvocadoView()->GetSelection ();
		if (selID>-1)
		{
			avocado::AvocadoMaterialInterface *ami = (GetActiveAvocadoDoc ()->GetElementMaterialData (selID));
			if (ami->textureFilename != string ("") && ami->textureFilename != "empty")
			{
				if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_TEXTURE,string(ami->textureFilename),string(ami->textureFilename)))
					m_wndRibbonBar.ForceRecalcLayout ();
			} else
			{
					::AfxMessageBox (_T("No current texture applied."));
			}
		}
	}
}


void CMainFrame::OnAvoacdogalleryEdittexturepreset()
{
if (theApp.m_galleryManager.EditPresets (ID_AVOACDOGALLERY_TEXTURE))
				m_wndRibbonBar.ForceRecalcLayout ();
	
}


void CMainFrame::OnAvoacdogalleryAddnewbumpgallery()
{
		if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_BUMP,"","",true))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryAddnewtexturegallery()
{
		if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_TEXTURE,"","",true))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresettexture()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOACDOGALLERY_TEXTURE))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresetbump()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOACDOGALLERY_BUMP))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresetmaterial()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOCADOVIEW_MATERIAL))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresetwood()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOACDOGALLERY_WOODBUMP))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryAddwoodgallery()
{
	if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_WOODBUMP,"","",true))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryEditwoodpreset()
{
	if (theApp.m_galleryManager.EditPresets (ID_AVOACDOGALLERY_WOODBUMP))
		m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryEditskinpreset()
{
	if (theApp.m_galleryManager.EditPresets (ID_AVOACDOGALLERY_SKIN))
		m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryAddnewskingallery()
{
	if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_SKIN,"","",true))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryEdithairpreset()
{
	if (theApp.m_galleryManager.EditPresets (ID_AVOACDOGALLERY_HAIR))
		m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryAddnewhairgallery()
{
	if (theApp.m_galleryManager.AddNewPreset (ID_AVOACDOGALLERY_HAIR,"","",true))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresetskin()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOACDOGALLERY_SKIN))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogalleryGalleryresethair()
{
	if (theApp.m_galleryManager.RestoreToDefault (ID_AVOACDOGALLERY_HAIR))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnAvoacdogallerySkin()
{
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_AVOACDOGALLERY_SKIN,data);
	int selID = GetActiveAvocadoView()->GetSelection ();
	string pr;
		std::stringstream pStr;

pStr <<  "int eid=" << selID
			<< ",string propid=" << data
			<< ",string propname=skintexturefilename"
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

}


void CMainFrame::OnLightsEditlights()
{
	m_wndRibbonBar.ShowContextCategories (ID_CONTEXT_LIGHTS,TRUE);
	m_wndRibbonBar.ActivateContextCategory(ID_CONTEXT_LIGHTS);
	m_wndRibbonBar.RecalcLayout();
}


void CMainFrame::OnLightsCloseeditlight()
{
	m_wndRibbonBar.ShowContextCategories (ID_CONTEXT_LIGHTS,FALSE);
	m_wndRibbonBar.RecalcLayout();
}

void CMainFrame::AvocadoMakeGroup (std::string groupType)
{
	vector<int> selList;
	int selCount = GetActiveAvocadoView()->GetSelectionCount ();
	if (selCount >1 )
	{
		// prepare list.
		for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
				selList.push_back (GetActiveAvocadoView()->GetSelection (selIDx));

		// 2 seperate phases. because in the future we might want to filter this list harder. I would not expect thousands of selections. but god knows.
		// prepare params.
		std::stringstream pStr;
		pStr << "string GroupType=" << groupType << ",";
		pStr << "int GroupChildrenCount=" << selList.size() << ",";
		for (int selIDx = 0;selIDx < int(selList.size());selIDx++)
		{
			pStr <<  "int GroupChildID" << selIDx << "=" << selList[selIDx] << ",";
		}
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddDocFileElement",GetActiveAvocadoDoc ()->GetAvoID (),string (pStr.str() + string ("bool isRef=0,bool isGroup=1,string geoName=new,int MetaCount=0,bool UpdateDocUI=1;")).c_str());
	}
}

void CMainFrame::OnAvocadoviewMakegroup()
{
	AvocadoMakeGroup("Kit");
}


void CMainFrame::OnUpdateAvocadoviewMakegroup(CCmdUI *pCmdUI)
{
	BOOL enable = FALSE;
	if (GetActiveDocument())
	{
		int selCount = GetActiveAvocadoView()->GetSelectionCount ();
		if (selCount > 1)
			enable = TRUE;
	}
	pCmdUI->Enable (enable);
}


void CMainFrame::OnInsertPoly()
{
	if (this->GetActiveDocument())
	{
		int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=Line;");
	}
}


void CMainFrame::OnInsertCube()
{
	if (this->GetActiveDocument())
	{

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_INSERT_CUBE,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
		int selidx = galel->GetSelectedItem ();
		if (selidx == 0)
		{
			OnInsertSimplecube();

		} else if (selidx == 1)
		{
			OnInsertCylinder();
		}
		else if (selidx == 2)
			{
				OnInsertSphere();
			}
		else if (selidx == 3)
			{
			 OnInsertPlane();
			}

	}
}


void CMainFrame::OnInsertSphere()
{
	int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
				GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=Sphere;");
}


void CMainFrame::OnInsertPlane()
{
		int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
				GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=Plane;");
}


void CMainFrame::OnInsertCylinder()
{
	
			int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=Cylinder;");
}


void CMainFrame::OnInsertSimplecube()
{
	int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=Cube;");
}


void CMainFrame::OnCheckboxRecordcamera()
{
	// TODO: Add your command handler code here
}


void CMainFrame::OnUpdateCheckboxRecordcamera(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CMainFrame::OnInsertSpotlight()
{
	int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,"string AnnotationType=SpotLight;");
}


//void CMainFrame::OnInsertPath()
//{
//	// TODO: Add your command handler code here
//}

void CMainFrame::OnInsertPathlinecomplex()
{
	int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			
			
			{
				if (GetActiveAvocadoView()->GetSelectionCount () <=1 )
				{
					OnInsertPathline();
				}
				else
				{
					// ADDING PAIRS.. DEPENDS ON SELECTION ORDER FOR NOW.
					for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ()-1;selIDx++)
					{ 
						int selID = (GetActiveAvocadoView()->GetSelection (selIDx));
						int selID2 = (GetActiveAvocadoView()->GetSelection (selIDx+1));
						std::stringstream pStr;
						pStr <<  "int AttachmentID=" << selID  <<  ",int AttachmentID2=" << selID2 << ",";
						pStr	<<"string AnnotationType=PathLine;";
						GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,pStr.str().c_str());
					}			
				}				
			}
}


void CMainFrame::OnInsertPathline2()
{
	OnInsertPathline();
}
void CMainFrame::OnInsertPathline()
{
		int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			
			
			{		
				for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
				{ 
					int selID = (GetActiveAvocadoView()->GetSelection (selIDx));
					std::stringstream pStr;
					pStr <<  "int AttachmentID=" << selID << "," <<"string AnnotationType=PathLine;";
					GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,pStr.str().c_str());
				}
			}
}


void CMainFrame::OnSliderSpecular2()
{
SetMaterialPropSlider ( ID_SLIDER_SPECULAR2, "specular");
}


void CMainFrame::OnUpdateSliderSpecular2(CCmdUI *pCmdUI)
{
FetchMaterialValueSlider(ID_SLIDER_SPECULAR2);
	 pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateFileImport2(CCmdUI *pCmdUI)
{
	pCmdUI->Enable ( (GetActiveDocument() != NULL)) ;
}


void CMainFrame::OnAvocadoviewMakegroup2()
{
	OnAvocadoviewMakegroup();
}

void CMainFrame::OnAvocadoviewMakeweld()
{
	AvocadoMakeGroup (string ("Weld"));
}

void CMainFrame::OnAvocadoviewAddtogroup()
{
	vector<int> selList;
	int selCount = GetActiveAvocadoView()->GetSelectionCount ();
	if (selCount >1 )
	{
		// prepare list.
		for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
				selList.push_back (GetActiveAvocadoView()->GetSelection (selIDx));

		// 2 seperate phases. because in the future we might want to filter this list harder. I would not expect thousands of selections. but god knows.
		// prepare params.
		std::stringstream pStr;
		for (int selIDx = 0;selIDx < int(selList.size());selIDx++)
		{
			pStr <<  "int eid" /*<< selIDx*/ << "=" << selList[selIDx] << ",";
		}
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddToGroup",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str().c_str());
	}
}


void CMainFrame::OnViewShowAllPanes()
{
	CreateDockingWindows(false);
}





void CMainFrame::OnInsertPropsgallery()
{
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_INSERT_PROPSGALLERY,data);
	wchar_t fileName[MAX_PATH]  ;
	::mbstowcs (fileName,data.c_str (),MAX_PATH);
	ImportFile  (fileName);

		////pStr <<  "int eid=" << selID
		//	<< ",string propid=" << data
		//	<< ",string propname=skintexturefilename"
		//	<< ";";
		//GetActiveAvocadoDoc()->AvocadoInvokeDoc("ChangeElementMaterialPropString",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());

}


void CMainFrame::OnInsertPropsadd()
{
	CString newName;
	if (!theApp.DoPromptImportFileName(theApp.m_importFilters,newName, AFX_IDS_OPENFILE,
	  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL,-1))
		return; // open cancelled
	char cname[MAX_PATH];

	::wcstombs (cname,newName.GetString (),MAX_PATH);

	if (theApp.m_galleryManager.AddNewPreset (ID_INSERT_PROPSGALLERY,std::string (cname),"",false))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnInsertPropsedit()
{
	if (theApp.m_galleryManager.EditPresets (ID_INSERT_PROPSGALLERY))
		m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnInsertPropsrestore()
{
		if (theApp.m_galleryManager.RestoreToDefault (ID_INSERT_PROPSGALLERY))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnLayoutGotopagegallery()
{
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_LAYOUT_GOTOPAGEGALLERY,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonGallery *galel = (CMFCRibbonGallery*)elb;
		int frameType  = galel->GetSelectedItem ();

		OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ",int page=" << frameType <<";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("GoToPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}


void CMainFrame::OnLayoutBackgrounimagegallry()
{
	string data;
	theApp.m_galleryManager.GetPresetValue (ID_LAYOUT_BACKGROUNIMAGEGALLRY,data);
	//wchar_t fileName[MAX_PATH]  ;
	//::mbstowcs (fileName,data.c_str (),MAX_PATH);
	string messa = string ("string backimage=")+data+string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.c_str());
}


void CMainFrame::OnLayoutBgimagegalleyadd()
{
		if (theApp.m_galleryManager.AddNewPreset (ID_LAYOUT_BACKGROUNIMAGEGALLRY,"","",true))
				m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnLayoutBgimagegalleryedit()
{
	if (theApp.m_galleryManager.EditPresets (ID_LAYOUT_BACKGROUNIMAGEGALLRY))
		m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnLayoutBgimagegalleryrestore()
{
		if (theApp.m_galleryManager.RestoreToDefault (ID_LAYOUT_BACKGROUNIMAGEGALLRY))
			m_wndRibbonBar.ForceRecalcLayout ();
}


void CMainFrame::OnRoomCylinder()
{
	if (GetActiveAvocadoDoc ())
	{
		string messa = string ("string roomType=")+string("cylinder")+string (";");
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "cylinder")
			messa = string ("string roomType=")+string("none")+string (";");
		
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.c_str());
	}
}


void CMainFrame::OnRoomSphere()
{
	if (GetActiveAvocadoDoc ())
	{
		string messa = string ("string roomType=")+string("sphere")+string (";");
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "sphere")
			messa = string ("string roomType=")+string("none")+string (";");
		
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.c_str());

	}
}


void CMainFrame::OnRoomCude()
{
	if (GetActiveAvocadoDoc ())
	{
		string messa = string ("string roomType=")+string("cube")+string (";");
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "cube")
			messa = string ("string roomType=")+string("none")+string (";");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.c_str());
	}
}


void CMainFrame::OnRoomTogglestatusbar()
{
	
	if (m_wndStatusBar.IsVisible ())
		m_wndStatusBar.ShowPane (FALSE,FALSE,FALSE);
	else
		m_wndStatusBar.ShowPane (TRUE,FALSE,FALSE);
}


void CMainFrame::OnSliderRoomscaleX()
{
	float val = float (GetRibbonSliderValue (ID_SLIDER_ROOMSCALE_X)) / 50.0f;

	stringstream messa ;
		messa <<   string ("string roomScaleX=") <<val<<string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnSliderRoomscaleY()
{
	float val = float (GetRibbonSliderValue (ID_SLIDER_ROOMSCALE_Y)) / 50.0f;

	stringstream messa ;
		messa <<   string ("string roomScaleY=") <<val<<string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnSliderRoomscaleZ()
{
	float val = float (GetRibbonSliderValue (ID_SLIDER_ROOMSCALE_Z)) / 50.0f;

	stringstream messa ;
		messa <<   string ("string roomScaleZ=") <<val<<string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnSliderGlobalExposure()
{
	float val = float (GetRibbonSliderValue (ID_SLIDER_GLOBAL_EXPOSURE)) / 5.0f;
	val -= 10.0f;

	stringstream messa ;
		messa <<   string ("string globalExposure=") <<val<<string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnUpdateSliderGlobalExposure(CCmdUI *pCmdUI)
{
	if (GetActiveDocument ())
	{
		pCmdUI->Enable (true);

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_SLIDER_GLOBAL_EXPOSURE,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalExposure");
		docvalue +=10.0f;
		docvalue *= 0.05f;
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
	
	
}


void CMainFrame::OnUpdateSliderRoomscaleX(CCmdUI *pCmdUI)
{
	if (GetActiveDocument ())
	{
		pCmdUI->Enable (true);

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_SLIDER_ROOMSCALE_X,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("roomScaleX");
		//docvalue +=10.0f;
		docvalue *= (50.f / 100.0f);
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}


void CMainFrame::OnUpdateSliderRoomscaleY(CCmdUI *pCmdUI)
{
	
	if (GetActiveDocument ())
	{
		pCmdUI->Enable (true);

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_SLIDER_ROOMSCALE_Y,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("roomScaleY");
		//docvalue +=10.0f;
		docvalue *= 0.5f;
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}


void CMainFrame::OnUpdateSliderRoomscaleZ(CCmdUI *pCmdUI)
{

	if (GetActiveDocument ())
	{
		pCmdUI->Enable (true);

		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (ID_SLIDER_ROOMSCALE_Z,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("roomScaleZ");
		//docvalue +=10.0f;
		docvalue *= 0.5f;
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));

	}
}

void CMainFrame::UpdateLightSlider (UINT cmd, string docParam,bool negScale)
{
	if (GetActiveDocument ())
	{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmd,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat (docParam);
		if (negScale)
		{
			docvalue += 1.0f;
			docvalue *= 0.5f;
		}
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));
	}
}

void CMainFrame::UpdateAnnotationSlider (UINT cmd, string docParam,bool negScale)
{
	if (GetActiveDocument ())
	{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr2;
		this->m_wndRibbonBar.GetElementsByID (cmd,arr2);
		CMFCRibbonBaseElement *elb = arr2.GetAt (0);
		CMFCRibbonSlider *galel = (CMFCRibbonSlider*)elb;
		 if (galel->IsPressed() || galel->IsFocused())//HasFocus ())
				 return;
	//float uivalue = float(galel->GetPos ()) / float(galel->GetRangeMax() - galel->GetRangeMin());
		 int selID = GetActiveAvocadoView()->GetSelection ();
		 if (selID > -1)
		 {
		 string sdocvalue = GetActiveAvocadoDoc ()->GetElementAnnotationData (selID,docParam);
		 if (sdocvalue != "")
		 {
		 float docvalue;
		 stringstream asr (sdocvalue);
			 asr >> docvalue;
		if (negScale)
		{
			docvalue += 1.0f;
			docvalue *= 0.5f;
		}
		if (docvalue <0.0f || docvalue >1.0f)
			return;
	//if ( fabs(uivalue - docvalue) > 0.1f)
		  galel->SetPos  (int(float(galel->GetRangeMin()) + docvalue  * float(abs(galel->GetRangeMax()-galel->GetRangeMin()))));
		 }

		 }
	}
}

void CMainFrame::SetAnnotationParamSlider (UINT cmd,string docParam,bool negScale)
{
	float val = float (GetRibbonSliderValue (cmd));
	if (negScale)
	{
		val /= 50.0f;
		val -= 1.0f;
	} else
	{
		val /= 100.f;
	}
	int selID = GetActiveAvocadoView()->GetSelection ();
	
	std::stringstream pStr;
	pStr <<  "int eid=" <<selID << ",string color=" << val << ",string ParamName=" << docParam << ";";
	//m_wndRibbonBar.ForceRecalcLayout();

	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetAnnotationParam",GetActiveAvocadoDoc ()->GetAvoID (), pStr.str().c_str());
}

void CMainFrame::SetLightValue (UINT cmd,string docParam,bool negScale)
{
	float val = float (GetRibbonSliderValue (cmd));
	if (negScale)
	{
		val /= 50.0f;
		val -= 1.0f;
	} else
	{
		val /= 100.f;
	}
	stringstream messa ;
		messa <<   string ("string ") << docParam << string ("=") <<val<<string (";");
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}



void CMainFrame::OnSliderLight1X()
{
	SetLightValue (ID_SLIDER_LIGHT1_X,"globalLight1X");
}


void CMainFrame::OnSliderLight1Y()
{
	SetLightValue (ID_SLIDER_LIGHT1_Y,"globalLight1Y");
}


void CMainFrame::OnSliderLight1Z()
{
	SetLightValue (ID_SLIDER_LIGHT1_Z,"globalLight1Z");
}


void CMainFrame::OnUpdateSliderLight1X(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT1_X,"globalLight1X");
}


void CMainFrame::OnUpdateSliderLight1Y(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT1_Y,"globalLight1Y");
}


void CMainFrame::OnUpdateSliderLight1Z(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT1_Z,"globalLight1Z");
}


void CMainFrame::OnSliderLight2X()
{
	SetLightValue (ID_SLIDER_LIGHT2_X,"globalLight2X");
}


void CMainFrame::OnSliderLight2Y()
{
	SetLightValue (ID_SLIDER_LIGHT2_Y,"globalLight2Y");
}


void CMainFrame::OnSliderLight2Z()
{
	SetLightValue (ID_SLIDER_LIGHT2_Z,"globalLight2Z");
}


void CMainFrame::OnUpdateSliderLight2X(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT2_X,"globalLight2X");
}


void CMainFrame::OnUpdateSliderLight2Y(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT2_Y,"globalLight2Y");
}


void CMainFrame::OnUpdateSliderLight2Z(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT2_Z,"globalLight2Z");
}


void CMainFrame::OnSliderLight1Cutoff()
{
	SetLightValue (ID_SLIDER_LIGHT1_CUTOFF,"globalLight1Intensity",false);
}


void CMainFrame::OnUpdateSliderLight1Cutoff(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT1_CUTOFF,"globalLight1Intensity",false);
}


void CMainFrame::OnCheckboxLight1Enabled()
{
	 CMFCRibbonBaseElement *galel = ( CMFCRibbonBaseElement*)GetRibbonBaseElement(ID_CHECKBOX_LIGHT1_ENABLED); 
	 float val = ( galel->IsChecked () ? 0.0f : 1.0f);
	 stringstream messa ;
		messa <<   string ("string ") << "globalLight1Enabled" << string ("=") <<val<<string (";");
	 GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnUpdateCheckboxLight1Enabled(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalLight1Enabled");
		int checkstate = (docvalue > 0.0f) ? 1 : 0;
		pCmdUI->SetCheck ( checkstate );
	}
	pCmdUI->Enable  (true);
}


void CMainFrame::OnSliderLight2Cutoff()
{
	SetLightValue (ID_SLIDER_LIGHT2_CUTOFF,"globalLight2Intensity",false);
}


void CMainFrame::OnUpdateSliderLight2Cutoff(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT2_CUTOFF,"globalLight2Intensity",false);
}


void CMainFrame::OnCheckboxLight2Enabled()
{
	 CMFCRibbonBaseElement *galel = ( CMFCRibbonBaseElement*)GetRibbonBaseElement(ID_CHECKBOX_LIGHT2_ENABLED); 
	 float val = ( galel->IsChecked () ? 0.0f : 1.0f);
	 stringstream messa ;
		messa <<   string ("string ") << "globalLight2Enabled" << string ("=") <<val<<string (";");
	 GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnUpdateCheckboxLight2Enabled(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalLight2Enabled");
		int checkstate = (docvalue > 0.0f) ? 1 : 0;
		pCmdUI->SetCheck ( checkstate );
	}
	pCmdUI->Enable  (true);
}


void CMainFrame::OnSliderLight3X()
{
	SetLightValue (ID_SLIDER_LIGHT3_X,"globalLight3X");
}


void CMainFrame::OnSliderLight3Y()
{
	SetLightValue (ID_SLIDER_LIGHT3_Y,"globalLight3Y");
}


void CMainFrame::OnSliderLight3Z()
{
	SetLightValue (ID_SLIDER_LIGHT3_Z,"globalLight3Z");
}


void CMainFrame::OnSliderLight3Cutoff()
{
	SetLightValue (ID_SLIDER_LIGHT3_CUTOFF,"globalLight3Intensity",false);
}


void CMainFrame::OnCheckboxLight3Enabled()
{
	CMFCRibbonBaseElement *galel = ( CMFCRibbonBaseElement*)GetRibbonBaseElement(ID_CHECKBOX_LIGHT3_ENABLED); 
	 float val = ( galel->IsChecked () ? 0.0f : 1.0f);
	 stringstream messa ;
		messa <<   string ("string ") << "globalLight3Enabled" << string ("=") <<val<<string (";");
	 GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
}


void CMainFrame::OnUpdateSliderLight3X(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT3_X,"globalLight3X");
}


void CMainFrame::OnUpdateSliderLight3Y(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT3_Y,"globalLight3Y");
}


void CMainFrame::OnUpdateSliderLight3Z(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT3_Z,"globalLight3Z");
}


void CMainFrame::OnUpdateSliderLight3Cutoff(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	UpdateLightSlider (ID_SLIDER_LIGHT3_CUTOFF,"globalLight3Intensity",false);
}


void CMainFrame::OnUpdateCheckboxLight3Enabled(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat ("globalLight3Enabled");
		int checkstate = (docvalue > 0.0f) ? 1 : 0;
		pCmdUI->SetCheck ( checkstate );
	}
	pCmdUI->Enable  (true);
}

void CMainFrame::SetSpotLightColorValue (UINT cmd,string docParam)
{
	
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	this->m_wndRibbonBar.GetElementsByID (cmd,arr);
	CMFCRibbonBaseElement *el = arr.GetAt (0);
	CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
	COLORREF cr = colel->GetColor();//GetHighlightedColor ();
	//LOBYTE(2)
//	GetGValue (s)
	int r = GetRValue (cr);
	int g = GetGValue (cr);
	int b = GetBValue (cr);
	//unsigned int rr = (unsigned int) ( r & 0xff );
	g = g << 8;
	b = b << 16;
	int selID = GetActiveAvocadoView()->GetSelection ();
	unsigned int colorID = (unsigned int )(unsigned long (r | g | b ));
	std::stringstream pStr;
	pStr <<  "int eid=" <<selID << ",string color=" << colorID << ",string ParamName=" << docParam << ";";
	//m_wndRibbonBar.ForceRecalcLayout();

	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetAnnotationParam",GetActiveAvocadoDoc ()->GetAvoID (), pStr.str().c_str());
}

void CMainFrame::SetDocParamColorValue (UINT cmd,string docParam)
{
	
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	this->m_wndRibbonBar.GetElementsByID (cmd,arr);
	CMFCRibbonBaseElement *el = arr.GetAt (0);
	CMFCRibbonColorButton *colel = (CMFCRibbonColorButton*)el;
	COLORREF cr = colel->GetColor();//GetHighlightedColor ();
	//LOBYTE(2)
//	GetGValue (s)
	int r = GetRValue (cr);
	int g = GetGValue (cr);
	int b = GetBValue (cr);
	//unsigned int rr = (unsigned int) ( r & 0xff );
	g = g << 8;
	b = b << 16;

	unsigned int colorID = (unsigned int )(unsigned long (r | g | b ));
	std::stringstream pStr;
	pStr <<  "string " << docParam << "=" << colorID << ";";
	//m_wndRibbonBar.ForceRecalcLayout();

	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), pStr.str().c_str());
}

void CMainFrame::OnLayoutGlobalambientcolor()
{
	SetDocParamColorValue (ID_LAYOUT_GLOBALAMBIENTCOLOR,"GlobalAmbientColor");
}


void CMainFrame::OnLightsLight1ambient()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT1AMBIENT,"light1AmbientColor");
}


void CMainFrame::OnLightsLight1diffuse()
{
SetDocParamColorValue (ID_LIGHTS_LIGHT1DIFFUSE,"light1DiffuseColor");
}


void CMainFrame::OnLightsLight1specular()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT1SPECULAR,"light1SpecularColor");
}


void CMainFrame::OnLightsLight2ambient()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT2AMBIENT,"light2AmbientColor");
}


void CMainFrame::OnLightsLight2diffuse()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT2DIFFUSE,"light2DiffuseColor");
}


void CMainFrame::OnLightsLight2specular()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT2SPECULAR,"light2SpecularColor");
}


void CMainFrame::OnLightsLight3ambient()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT3AMBIENT,"light3AmbientColor");
}


void CMainFrame::OnLightsLight3diffuse()
{
SetDocParamColorValue (ID_LIGHTS_LIGHT3DIFFUSE,"light3DiffuseColor");
}


void CMainFrame::OnLightsLight3specular()
{
	SetDocParamColorValue (ID_LIGHTS_LIGHT3SPECULAR,"light3SpecularColor");
}

void CMainFrame::OnLightsSpotambient()
{
	SetSpotLightColorValue(ID_LIGHTS_SPOTAMBIENT,"SpotLightAmbient");
}


void CMainFrame::OnLightsSpotdiffuse()
{
	SetSpotLightColorValue(ID_LIGHTS_SPOTDIFFUSE,"SpotLightDiffuse");
}


void CMainFrame::OnLightsSpotspecular()
{
	SetSpotLightColorValue(ID_LIGHTS_SPOTSPECULAR,"SpotLightSpecular");
}


void CMainFrame::OnSliderSpotAttunuation()
{
	SetAnnotationParamSlider (ID_SLIDER_SPOT_ATTUNUATION,"SpotLightAttunuation",false);
}


void CMainFrame::OnSliderSpotIntensity()
{
	SetAnnotationParamSlider (ID_SLIDER_SPOT_INTENSITY,"SpotLightIntensity",false);
}


void CMainFrame::OnSliderSpotExponent()
{
	SetAnnotationParamSlider (ID_SLIDER_SPOT_EXPONENT,"SpotLightExponent",false);
}


void CMainFrame::OnUpdateSliderSpotExponent(CCmdUI *pCmdUI)
{
	UpdateAnnotationSlider(ID_SLIDER_SPOT_EXPONENT,"SpotLightExponent",false);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderSpotAttunuation(CCmdUI *pCmdUI)
{
	UpdateAnnotationSlider(ID_SLIDER_SPOT_ATTUNUATION,"SpotLightAttunuation",false);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderSpotIntensity(CCmdUI *pCmdUI)
{
	UpdateAnnotationSlider(ID_SLIDER_SPOT_INTENSITY,"SpotLightIntensity",false);
	pCmdUI->Enable (true);
}


void CMainFrame::OnSliderSpotLinear()
{
		SetAnnotationParamSlider (ID_SLIDER_SPOT_LINEAR,"SpotLightLinear",false);
}


void CMainFrame::OnSliderSpotQuadric()
{
	SetAnnotationParamSlider (ID_SLIDER_SPOT_QUADRIC,"SpotLightQuadric",false);
}


void CMainFrame::OnUpdateSliderSpotLinear(CCmdUI *pCmdUI)
{
	UpdateAnnotationSlider(ID_SLIDER_SPOT_LINEAR,"SpotLightLinear",false);
	pCmdUI->Enable (true);
}


void CMainFrame::OnUpdateSliderSpotQuadric(CCmdUI *pCmdUI)
{
	UpdateAnnotationSlider(ID_SLIDER_SPOT_QUADRIC,"SpotLightQuadric",false);
	pCmdUI->Enable (true);
}


void CMainFrame::OnSliderIor()
{
	SetMaterialPropSlider ( ID_SLIDER_IOR, "ior");
}


void CMainFrame::OnUpdateSliderIor(CCmdUI *pCmdUI)
{
	FetchMaterialValueSlider(ID_SLIDER_IOR);
	pCmdUI->Enable (true);

}


void CMainFrame::OnUpdateRoomCylinder(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "cylinder")
			pCmdUI->SetCheck (TRUE);
		else
			pCmdUI->SetCheck (FALSE);
	}
}


void CMainFrame::OnUpdateRoomCude(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "cube")
			pCmdUI->SetCheck (TRUE);
		else
			pCmdUI->SetCheck (FALSE);
	}
}


void CMainFrame::OnUpdateRoomSphere(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc ())
	{
		string docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsString("roomType");
		if (docvalue == "sphere")
			pCmdUI->SetCheck (TRUE);
		else
			pCmdUI->SetCheck (FALSE);
	}
}


void CMainFrame::OnFileOptions()
{
	theApp.InitOptionsDialog ();
}




void CMainFrame::OnMaterialstateCombo()
{
	static int lastSel = -1;
	CMFCRibbonComboBox *com;
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	m_wndRibbonBar.GetElementsByID (ID_MATERIALSTATE_COMBO,arr);
	com = (CMFCRibbonComboBox*) ((CMFCRibbonBaseElement*)arr.GetAt (0));
	int curSel = com->GetCurSel ();
	if (curSel != lastSel)
	{
		lastSel = curSel;
	stringstream messa;
	messa << "int materialStateID=" << curSel << ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SwitchToMaterialState",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
	}
}


void CMainFrame::OnMaterialstateAdd()
{
	// TODO: Add your command handler code here
 	int sel = GetActiveAvocadoView()->GetSelection ();
	if (sel < 0 && (GetActiveAvocadoView()->GetSelectionCount () <= 0) )
		return;
	if (GetActiveAvocadoView()->GetSelectionCount () > 1)
			sel = -1;
	std::vector<int> sellist ;
	stringstream messa;
	for (int selIdx=0; selIdx < GetActiveAvocadoView()->GetSelectionCount () ;selIdx++)
	{
			sellist.push_back (GetActiveAvocadoView()->GetSelection (selIdx));
			messa << "int eid=" << GetActiveAvocadoView()->GetSelection (selIdx) << ",";
	}

	bool exists = true;
	CString res = CString ("");
	while (exists == true)
	{
		res = theApp.DoPromptInputDlg (CString (L"Add Material State"),CString(L"Enter name for new material state:"));
		if (res == CString (""))
			break;
		exists = false;
		for (size_t kms = 0; kms < GetActiveAvocadoDoc()->GetMaterialStates ().size ();kms++)
		{
			CString msdd (GetActiveAvocadoDoc()->GetMaterialStates ()[kms].c_str());
			if (msdd == res)
			{
			  exists = true;
			  theApp.DoMessageBox (L"Material State already exists",MB_OK,NULL);
			}
		}
	}
	if (res != CString (""))
	{
		char buf[MAX_PATH];
		::wcstombs (buf,res.GetString (),MAX_PATH);
		//messa << "string name=NewMaterialState" << GetActiveAvocadoDoc()->GetMaterialStates ().size() << ";";
		messa << "string name="<< string (buf) << ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddMaterialState",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
	}
}


void CMainFrame::OnUpdateMaterialstateAdd(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	bool enabled = true;
	int sel = GetActiveAvocadoView()->GetSelection ();
	if (sel < 0 && (GetActiveAvocadoView()->GetSelectionCount () < 1) )
		enabled = false;
	pCmdUI->Enable (enabled);
}


void CMainFrame::OnMaterialstateDelete()
{
	// TODO: Add your command handler code here
	CMFCRibbonComboBox *com;
	CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
	m_wndRibbonBar.GetElementsByID (ID_MATERIALSTATE_COMBO,arr);
	com = (CMFCRibbonComboBox*) ((CMFCRibbonBaseElement*)arr.GetAt (0));
	int cur_sel = com->GetCurSel();
	stringstream messa;
	messa << "int materialStateID=" << cur_sel << ";";
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("DeleteMaterialState",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());

}


void CMainFrame::OnUpdateMaterialstateDelete(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}
void CMainFrame::RefreshMaterialStates ()
{
	CMFCRibbonComboBox *com;
	if (GetActiveAvocadoDoc())
	{
		CArray <CMFCRibbonBaseElement*,CMFCRibbonBaseElement*> arr;
		m_wndRibbonBar.GetElementsByID (ID_MATERIALSTATE_COMBO,arr);
		if (arr.IsEmpty())
			return;
		com = (CMFCRibbonComboBox*) ((CMFCRibbonBaseElement*)arr.GetAt (0));
		std::vector<string> msv = GetActiveAvocadoDoc()->GetMaterialStates ();
		com->RemoveAllItems ();
		for (size_t i=0;i<msv.size();i++)
		{
			wchar_t vuf[MAX_PATH];
			::mbstowcs (vuf,msv[i].c_str(),MAX_PATH);
			com->AddItem (vuf);
		}
	}
}

void CMainFrame::OnMaterialstateAddexisting()
{
	if (GetActiveAvocadoDoc())
	{
		std::vector<string> msv = GetActiveAvocadoDoc()->GetMaterialStates ();
		int ind = theApp.RaiseMaterialStateDlg (msv);
		int sel = GetActiveAvocadoView()->GetSelection ();
		if (sel < 0 && (GetActiveAvocadoView()->GetSelectionCount () <= 0) )
			return;
		if (GetActiveAvocadoView()->GetSelectionCount () > 1)
				sel = -1;
		std::vector<int> sellist ;
		stringstream messa;
		for (int selIdx=0; selIdx < GetActiveAvocadoView()->GetSelectionCount () ;selIdx++)
		{
				sellist.push_back (GetActiveAvocadoView()->GetSelection (selIdx));
				messa << "int eid=" << GetActiveAvocadoView()->GetSelection (selIdx) << ",";
		}
		//messa << "string name=NewMaterialState" << GetActiveAvocadoDoc()->GetMaterialStates ().size() << ";";
		messa << "int index="<<ind<<";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddToMaterialState",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
	
	}
}


void CMainFrame::OnUpdateMaterialstateAddexisting(CCmdUI *pCmdUI)
{
	if (GetActiveAvocadoDoc() && GetActiveAvocadoView())
	{
		int sel = GetActiveAvocadoView()->GetSelection ();
	    if (sel < 0 && (GetActiveAvocadoView()->GetSelectionCount () <= 0) )
		{
			pCmdUI->Enable (FALSE);
		}
		else
		{
			std::vector<string> msv = GetActiveAvocadoDoc()->GetMaterialStates ();
			if (msv.empty())
				pCmdUI->Enable (FALSE);
			else
				pCmdUI->Enable (TRUE);
		}
	}
}


void CMainFrame::OnSliderEffectBright()
{
  SetLightValue(ID_SLIDER_EFFECT_BRIGHT,"PostEffectBrightness",false);
}


void CMainFrame::OnSliderEffectCont()
{
	SetLightValue(ID_SLIDER_EFFECT_CONT,"PostEffectContrast",false);
	// TODO: Add your command handler code here
}


void CMainFrame::OnUpdateSliderEffectBright(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
		pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectBrightnessEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_BRIGHT,"PostEffectBrightness",false);
}


void CMainFrame::OnUpdateSliderEffectCont(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectBrightnessEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_CONT,"PostEffectContrast",false);

}


void CMainFrame::OnSliderEffectHue()
{
	// TODO: Add your command handler code here
	 SetLightValue(ID_SLIDER_EFFECT_HUE,"PostEffectHue",false);
}


void CMainFrame::OnSliderEffectSaturate()
{
	 SetLightValue(ID_SLIDER_EFFECT_SATURATE,"PostEffectSaturation",false);
}


void CMainFrame::OnSliderEffectNoise()
{
	 SetLightValue(ID_SLIDER_EFFECT_NOISE,"PostEffectNoise",false);
}


void CMainFrame::OnUpdateSliderEffectNoise(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectNoiseEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_NOISE,"PostEffectNoise",false);
}


void CMainFrame::OnSliderEffectSoften()
{
	// TODO: Add your command handler code here
	SetLightValue(ID_SLIDER_EFFECT_SOFTEN,"PostEffectSoften",false);
}


void CMainFrame::OnUpdateSliderEffectSoften(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectSoftenEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_SOFTEN,"PostEffectSoften",false);
}


void CMainFrame::OnUpdateSliderEffectHue(CCmdUI *pCmdUI)
{
		pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectSaturationEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_HUE,"PostEffectHue",false);
}


void CMainFrame::OnUpdateSliderEffectSaturate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectSaturationEnabled"));
	UpdateLightSlider (ID_SLIDER_EFFECT_SATURATE,"PostEffectSaturation",false);
}

void CMainFrame::SetDocParamCheckBox (UINT cmdid , std::string param)
{
	if (GetActiveAvocadoDoc())
	{
		CMFCRibbonBaseElement *galel = ( CMFCRibbonBaseElement*)GetRibbonBaseElement(cmdid); 
		float val = ( galel->IsChecked () ? 0.0f : 1.0f);
		stringstream messa ;
		messa <<   string ("string ") << param << string ("=") <<val<<string (";");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), messa.str().c_str());
	}
}

bool CMainFrame::UpdateDocParamCheckBox (std::string param)
{
	if (GetActiveAvocadoDoc())
	{
		float docvalue = GetActiveAvocadoDoc ()->GetDocParameterAsFloat (param);
		if (docvalue > 0.0) return true;
		return false;
	}
	return false;
}

void CMainFrame::OnCheckEffectSaturate()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_SATURATE,"PostEffectSaturationEnabled");
}


void CMainFrame::OnUpdateCheckEffectSaturate(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectSaturationEnabled"));
}


void CMainFrame::OnCheckEffectBright()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_BRIGHT,"PostEffectBrightnessEnabled");
}


void CMainFrame::OnUpdateCheckEffectBright(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectBrightnessEnabled"));
}


void CMainFrame::OnCheckEffectNoise()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_NOISE,"PostEffectNoiseEnabled");
}


void CMainFrame::OnUpdateCheckEffectNoise(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectNoiseEnabled"));
}


void CMainFrame::OnCheckEffectSoften()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_SOFTEN,"PostEffectSoftenEnabled");
}


void CMainFrame::OnUpdateCheckEffectSoften(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectSoftenEnabled"));
}


void CMainFrame::OnButtonEffectPost()
{
	SetDocParamCheckBox (ID_BUTTON_EFFECT_POST,"PostEffectEnabled");
}


void CMainFrame::OnUpdateButtonEffectPost(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectEnabled"));
}


void CMainFrame::OnCheckEffectAo()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_AO,"PostEffectAOEnabled");
}


void CMainFrame::OnUpdateCheckEffectAo(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectAOEnabled"));
}


void CMainFrame::OnCheckEffectGamma()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_GAMMA,"PostEffectGammaEnabled");
}


void CMainFrame::OnCheckEffectTonemap()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_TONEMAP,"PostEffectToneMapEnabled");
}


void CMainFrame::OnCheckEffectFxaa()
{
	SetDocParamCheckBox (ID_CHECK_EFFECT_FXAA,"PostEffectFXAA");
}


void CMainFrame::OnUpdateCheckEffectGamma(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectGammaEnabled"));
}


void CMainFrame::OnUpdateCheckEffectTonemap(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectToneMapEnabled"));
}


void CMainFrame::OnUpdateCheckEffectFxaa(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectFXAA"));
}


void CMainFrame::OnCheckEffectDof()
{
		SetDocParamCheckBox (ID_CHECK_EFFECT_DOF,"PostEffectDOFEnabled");
}


void CMainFrame::OnUpdateCheckEffectDof(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (UpdateDocParamCheckBox ("PostEffectDOFEnabled"));
}


void CMainFrame::OnSliderEffectDofplane()
{
	 SetLightValue(ID_SLIDER_EFFECT_DOFPLANE,"PostEffectDOFPlane",false);
}


void CMainFrame::OnUpdateSliderEffectDofplane(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectDOFEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_DOFPLANE,"PostEffectDOFPlane",false);
}


void CMainFrame::OnSliderEffectGamma()
{
	 SetLightValue(ID_SLIDER_EFFECT_GAMMA,"PostEffectGamma",false);
}


void CMainFrame::OnUpdateSliderEffectGamma(CCmdUI *pCmdUI)
{
	   pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectGammaEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_GAMMA,"PostEffectGamma",false);
}


void CMainFrame::OnSliderEffectTonemap()
{
		 SetLightValue(ID_SLIDER_EFFECT_TONEMAP,"PostEffectToneMap",false);
}


void CMainFrame::OnUpdateSliderEffectTonemap(CCmdUI *pCmdUI)
{
     	pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectToneMapEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_TONEMAP,"PostEffectToneMap",false);
}


void CMainFrame::OnSliderEffectAointensity()
{
	 SetLightValue(ID_SLIDER_EFFECT_AOINTENSITY,"PostEffectAOIntensity",false);
}


void CMainFrame::OnUpdateSliderEffectAointensity(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (UpdateDocParamCheckBox ("PostEffectAOEnabled"));
	   UpdateLightSlider (ID_SLIDER_EFFECT_AOINTENSITY,"PostEffectAOIntensity",false);
}


void CMainFrame::OnInsertCallout()
{
		int id = ((CAvocadoDoc*)(this->GetActiveDocument()))->GetAvoID();
			
			
			{		
				for (int selIDx = 0;selIDx <  GetActiveAvocadoView()->GetSelectionCount ();selIDx++)
				{ 
					int selID = (GetActiveAvocadoView()->GetSelection (selIDx));
					std::stringstream pStr;
					pStr <<  "int AttachmentID=" << selID << "," <<"string AnnotationType=Callout;";
					GetActiveAvocadoDoc()->AvocadoInvokeDoc("AddAnnotationElement",id,pStr.str().c_str());
				}
			}
}


void CMainFrame::OnExportpagePng()
{
	ExportDialog (5);
}


void CMainFrame::OnExportpageTiff()
{
	ExportDialog (4);
}
