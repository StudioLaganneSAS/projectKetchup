
# Add include directories
include_directories("${PROJECT_SOURCE_DIR}/src/core" 
					"${PROJECT_SOURCE_DIR}/src/tools"
					"${PROJECT_SOURCE_DIR}/src/vector")

				
set(EXTRA_SOURCES) 

if(APPLE)

list(APPEND EXTRA_SOURCES PKBitmapOpsPPC.cpp) 
list(APPEND EXTRA_SOURCES PKBitmapOpsPPC.h)
list(APPEND EXTRA_SOURCES PKMacCustomView.cpp)
list(APPEND EXTRA_SOURCES PKMacCustomView.h)
list(APPEND EXTRA_SOURCES PKMacSegmentedToolbarItem.cpp)
list(APPEND EXTRA_SOURCES PKMacSegmentedToolbarItem.h)
list(APPEND EXTRA_SOURCES PKMacSegmentedView.cpp)
list(APPEND EXTRA_SOURCES PKMacSegmentedView.h)
list(APPEND EXTRA_SOURCES PKMacView.cpp)
list(APPEND EXTRA_SOURCES PKMacView.h)
list(APPEND EXTRA_SOURCES PKCarbonEvent.cpp)
list(APPEND EXTRA_SOURCES PKCarbonEvent.h)

endif(APPLE)					
					
# Add tools library
add_library(ui 

PKAlertDialog.cpp
PKAlertDialog.h
PKAnimationControl.cpp
PKAnimationControl.h
PKApp.cpp
PKApp.h
PKArrowTabControl.cpp
PKArrowTabControl.h
PKBitmapBorderVectorPanel.cpp
PKBitmapBorderVectorPanel.h
PKBitmapButton.cpp
PKBitmapButton.h
PKBitmapLabel.cpp
PKBitmapLabel.h
PKBitmapOps.cpp
PKBitmapOps.h
PKBitmapOpsFunctions.h
PKBitmapOpsX86.h
PKBitmapProgressBar.cpp
PKBitmapProgressBar.h
PKBitmapToggleButton.cpp
PKBitmapToggleButton.h
PKBlitter.cpp
PKBlitter.h
PKButton.cpp
PKButton.h
PKCheckBox.cpp
PKCheckBox.h
PKChooseFolderDialog.cpp
PKChooseFolderDialog.h
PKComboBox.cpp
PKComboBox.h
PKControl.cpp
PKControl.h
PKCustomContainer.cpp
PKCustomContainer.h
PKCustomControl.cpp
PKCustomControl.h
PKDialog.cpp
PKDialog.h
PKDoubleBufferedControl.cpp
PKDoubleBufferedControl.h
PKEdit.cpp
PKEdit.h
PKEditableVectorLabel.cpp
PKEditableVectorLabel.h
PKFixedLayout.cpp
PKFixedLayout.h
PKFont.cpp
PKFont.h
PKGradientPanel.cpp
PKGradientPanel.h
PKGridLayout.cpp
PKGridLayout.h
PKGroupBox.cpp
PKGroupBox.h
PKHLayout.cpp
PKHLayout.h
PKIconTabControl.cpp
PKIconTabControl.h
PKImageVectorControl.cpp
PKImageVectorControl.h
PKImageView.cpp
PKImageView.h
PKLabel.cpp
PKLabel.h
PKLayout.cpp
PKLayout.h
PKLineControl.cpp
PKLineControl.h
PKLink.cpp
PKLink.h
PKListBox.cpp
PKListBox.h
PKListView.cpp
PKListView.h
PKLittleArrows.cpp
PKLittleArrows.h
PKMenu.cpp
PKMenu.h
PKMenuItem.cpp
PKMenuItem.h
PKModalDialog.cpp
PKModalDialog.h
PKOpenDialog.cpp
PKOpenDialog.h
PKPanelControl.cpp
PKPanelControl.h
PKPopupMenu.cpp
PKPopupMenu.h
PKProgressBar.cpp
PKProgressBar.h
PKSaveDialog.cpp
PKSaveDialog.h
PKScrollView.cpp
PKScrollView.h
PKSelectorControl.cpp
PKSelectorControl.h
PKSeparator.cpp
PKSeparator.h
PKSlider.cpp
PKSlider.h
PKSpacer.cpp
PKSpacer.h
PKStretchButton.cpp
PKStretchButton.h
PKStretchEdit.cpp
PKStretchEdit.h
PKStretchToggleButton.cpp
PKStretchToggleButton.h
PKSwitchControl.cpp
PKSwitchControl.h
PKTabControl.cpp
PKTabControl.h
PKToolbar.cpp
PKToolbar.h
PKToolbarFixedSpace.cpp
PKToolbarFixedSpace.h
PKToolbarFlexibleSpace.cpp
PKToolbarFlexibleSpace.h
PKToolbarItem.cpp
PKToolbarItem.h
PKToolbarSegmentedItem.cpp
PKToolbarSegmentedItem.h
PKToolbarSeparator.cpp
PKToolbarSeparator.h
PKTreeView.cpp
PKTreeView.h
PKUI.cpp
PKUI.h
PKUIFactory.cpp
PKUIFactory.h
PKVLayout.cpp
PKVLayout.h
PKVectorAnimation.cpp
PKVectorAnimation.h
PKVectorAnimationBlock.cpp
PKVectorAnimationBlock.h
PKVectorAnimationDescriptor.cpp
PKVectorAnimationDescriptor.h
PKVectorAnimationDescriptorFade.cpp
PKVectorAnimationDescriptorFade.h
PKVectorAnimationDescriptorFadeTo.cpp
PKVectorAnimationDescriptorFadeTo.h
PKVectorAnimationDescriptorSlide.cpp
PKVectorAnimationDescriptorSlide.h
PKVectorControl.cpp
PKVectorControl.h
PKVectorControlRoot.cpp
PKVectorControlRoot.h
PKVectorView.cpp
PKVectorView.h
PKView.cpp
PKView.h
PKWindow.cpp
PKWindow.h

${EXTRA_SOURCES}

)