// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright The XCSoar Project

#include "InfoBoxesConfigPanel.hpp"
//#include "../dlgConfigInfoboxes.hpp"
#include "Profile/Profile.hpp"
#include "Profile/Current.hpp"
#include "Profile/InfoBoxConfig.hpp"
#include "Form/Button.hpp"

#include "Form/CheckBox.hpp"

#include "Interface.hpp"
#include "InfoBoxes/InfoBoxManager.hpp"
#include "InfoBoxes/InfoBoxLayout.hpp"
#include "Widget/RowFormWidget.hpp"
#include "Language/Language.hpp"
#include "UIGlobals.hpp"
#include "Look/Look.hpp"

#include "Form/DataField/Enum.hpp"
#include "Form/DataField/Listener.hpp"
#include "InfoBoxes/Content/Factory.hpp"
#include "Screen/Layout.hpp"


#include <stdio.h>

class InfoBoxesConfigPanel;

/**
 * Paint a preview of an infobox 
 */
class InfoBoxPreview : public PaintWindow {
  InfoBoxesConfigPanel *parent;
  unsigned i;

public:
  void SetParent(InfoBoxesConfigPanel &_parent, unsigned _i) {
    parent = &_parent;
    i = _i;
  }

protected:
  /* virtual methods from class Window */
  bool OnMouseDown(PixelPoint p) noexcept override;
  bool OnMouseDouble(PixelPoint p) noexcept override;

  /* virtual methods from class PaintWindow */
  void OnPaint(Canvas &canvas) noexcept override;
};




class InfoBoxesConfigPanel final
  : public RowFormWidget, DataFieldListener {

  StaticArray<InfoBoxPreview, InfoBoxSettings::Panel::MAX_CONTENTS> previews;
  unsigned                        current_preview = 0;
  const InfoBoxSettings           &settings       = CommonInterface::GetUISettings().info_boxes;

  const InfoBoxSettings::Geometry geometry        = InfoBoxManager::layout.geometry;

 
  struct Layout {
    InfoBoxLayout::Layout info_boxes;

    PixelRect form;

    PixelRect swap_chkbox, copy_button, paste_button, close_button;

    Layout(PixelRect rc, InfoBoxSettings::Geometry geometry);
  };


public:
  InfoBoxesConfigPanel()
    :RowFormWidget(UIGlobals::GetDialogLook()) {}

public:
  void Prepare(ContainerWindow &parent, const PixelRect &rc) noexcept override;
  bool Save(bool &changed) noexcept override;

  void OnAction(int id) noexcept;

  void SetCurrentInfoBox(unsigned _current_preview);

  void BeginEditing() { 
    //GetControl(BOX_NAME).BeginEditing();
  }
  
  void Show(const PixelRect &rc) noexcept override;

private:
  Button copy_button, paste_button, close_button;
  CheckBoxControl swap_chkbox;

  void
  OnModified([[maybe_unused]] DataField &df) noexcept override {
    /*
    if (IsDataField(BOX_POS, df)) {
      const DataFieldEnum &dfe = (const DataFieldEnum &)df;
      SetCurrentInfoBox(dfe.GetValue());
    } else if (IsDataField(BOX_NAME, df)) {
      const DataFieldEnum &dfe = (const DataFieldEnum &)df;

      auto new_value = (InfoBoxFactory::Type)dfe.GetValue();
      if (new_value == data.contents[current_preview])
        return;

      changed = true;
      data.contents[current_preview] = new_value;
      previews[current_preview].Invalidate();
      RefreshEditContentDescription();
    }
    */
  };

  unsigned
  GetCurrentInfoBox() const {
    return current_preview;
  }

  InfoBoxFactory::Type 
  GetContents(unsigned k, unsigned i) const {
     return settings.panels[k].contents[i];
    //return data.contents[i];
  }
  

};


void InfoBoxesConfigPanel::Show(const PixelRect &rc) noexcept {
  const Layout layout(rc, geometry);

  RowFormWidget::Show(layout.form);

  /*
  copy_button.MoveAndShow(layout.copy_button);
  paste_button.MoveAndShow(layout.paste_button);
  close_button.MoveAndShow(layout.close_button);
  swap_chkbox.MoveAndShow(layout.swap_chkbox);
  */
  for (unsigned i = 0; i < previews.size(); ++i) {
    previews[i].MoveAndShow(layout.info_boxes.positions[i]);
  }
}


InfoBoxesConfigPanel::Layout::Layout(PixelRect rc,
                                      InfoBoxSettings::Geometry geometry)
{
  info_boxes = InfoBoxLayout::Calculate(rc, geometry);

  // Screen W x H = 559 x 462
  const PixelSize box0 = rc.GetSize();
  printf("Screen W x H = %d x %d\n", box0.width, box0.height);


  form = info_boxes.remaining;
  printf("Form (%d, %d, %d, %d)\n", form.left, form.top, form.right, form.bottom);

  
  auto buttons = form.CutTopSafe(::Layout::GetMaximumControlHeight());

  swap_chkbox = copy_button = paste_button = close_button = buttons;

  int L = buttons.left;
  int W = buttons.right-L;

  swap_chkbox.left   = L;
  swap_chkbox.right  = copy_button.left  = L + (1*W)/4;
  copy_button.right  = paste_button.left = L + (2*W)/4;
  paste_button.right = close_button.left = L + (3*W)/4;
  close_button.right = L + W;
}

void
InfoBoxesConfigPanel::SetCurrentInfoBox(unsigned _current_preview)
{
  assert(_current_preview < previews.size());
  /*

  if (_current_preview == current_preview) {
    swap_chkbox.SetState(false);
    return;
  }

  bool is_checked = swap_chkbox.GetState();

  previews[current_preview].Invalidate();
  if ( is_checked ) {
    // Swap selected Infobox with the previous one.
    InfoBoxFactory::Type targetBox = data.contents[_current_preview];
    data.contents[_current_preview] = data.contents[current_preview];
    data.contents[current_preview] = targetBox;
    changed = true;
  }
  else {
    // Make selected InfoBox current.
    current_preview = _current_preview;
  }
  previews[current_preview].Invalidate();

  LoadValueEnum(BOX_POS, current_preview);

  RefreshEditContent();
  RefreshEditContentDescription();
  */
}



void
InfoBoxesConfigPanel::OnAction([[maybe_unused]] int id) noexcept
{
  /*
  InfoBoxSettings &settings = CommonInterface::SetUISettings().info_boxes;

  unsigned i = (unsigned)id;

  InfoBoxSettings::Panel &data = settings.panels[i];
  bool changed =
    dlgConfigInfoboxesShowModal(UIGlobals::GetMainWindow(),
                                UIGlobals::GetDialogLook(),
                                UIGlobals::GetLook().info_box,
                                InfoBoxManager::layout.geometry, data,
                                i >= InfoBoxSettings::PREASSIGNED_PANELS);
  if (changed) {
    Profile::Save(Profile::map, data, i);
    Profile::Save();
    ((Button &)GetRow(i)).SetCaption(gettext(data.name));
  }
  */
}

void
InfoBoxesConfigPanel::Prepare(ContainerWindow &parent,
                              const PixelRect &rc) noexcept
{
  //const InfoBoxSettings &settings = CommonInterface::GetUISettings().info_boxes;

  //RowFormWidget::Prepare(parent, rc);
    
  const Layout layout(rc, geometry);

  WindowStyle preview_style;
  preview_style.Hide();

  DataFieldEnum *dfe1 = new DataFieldEnum(this);
  for (unsigned i = 0; i < layout.info_boxes.count; ++i) {
    TCHAR label[32];
    _stprintf(label, _T("%u"), i + 1);
    dfe1->addEnumText(label, i);
  }

  previews.resize(layout.info_boxes.count);
  for (unsigned i = 0; i < layout.info_boxes.count; ++i) {
    previews[i].SetParent(*this, i);
    previews[i].Create(parent, layout.info_boxes.positions[i],
                       preview_style);
  }


  // Show InfoBox Sets in a dropdown list
  DataFieldEnum * dfes = new DataFieldEnum(this);
  for (unsigned i = 0; i < InfoBoxSettings::MAX_PANELS; i++) {
    const InfoBoxSettings::Panel &data = settings.panels[i];
    const TCHAR *name = gettext(data.name);
    if (name != NULL)
      dfes->addEnumText(gettext(name), i, gettext(name));
    //AddButton(gettext(data.name), [this, i](){ OnAction(i); });
    //if (i>2)
    //  SetExpertRow(i);
  }
  Add(_("InfoBox Set"), nullptr, dfes);

  
  // Show InfoBoxes in a dropdown list
  DataFieldEnum * dfe = new DataFieldEnum(this);
  for (unsigned i = InfoBoxFactory::MIN_TYPE_VAL; i < InfoBoxFactory::NUM_TYPES; i++) {
    const TCHAR *name = InfoBoxFactory::GetName((InfoBoxFactory::Type) i);
    const TCHAR *desc = InfoBoxFactory::GetDescription((InfoBoxFactory::Type) i);
    if (name != NULL)
      dfe->addEnumText(gettext(name), i, desc != NULL ? gettext(desc) : NULL);
  }
  dfe->EnableItemHelp(true);
  dfe->Sort(0);
  Add(_("InfoBox"), nullptr, dfe);
  

  AddBoolean(_("Use final glide mode"),
             _("Controls whether the \"final glide\" InfoBox mode should be used on \"auto\" pages."),
             settings.use_final_glide);
}

bool
InfoBoxesConfigPanel::Save([[maybe_unused]] bool &_changed) noexcept
{
  InfoBoxSettings &settings = CommonInterface::SetUISettings().info_boxes;
  SaveValue(InfoBoxSettings::MAX_PANELS, ProfileKeys::UseFinalGlideDisplayMode,
            settings.use_final_glide);

  return true;
}




/* virtual methods from class Window */
bool InfoBoxPreview::OnMouseDown([[maybe_unused]] PixelPoint p) noexcept {
  parent->SetCurrentInfoBox(i);
  return true;
}

bool InfoBoxPreview::OnMouseDouble([[maybe_unused]] PixelPoint p) noexcept {
  parent->BeginEditing();
  return true;
}

/* virtual methods from class PaintWindow */
void InfoBoxPreview::OnPaint([[maybe_unused]] Canvas &canvas) noexcept
{
  /*
  const bool is_current = i == parent->GetCurrentInfoBox();

  if (is_current)
    canvas.Clear(COLOR_BLACK);
  else
    canvas.ClearWhite();

  canvas.SelectHollowBrush();
  canvas.SelectBlackPen();
  canvas.DrawRectangle(PixelRect{PixelSize{canvas.GetWidth() - 1, canvas.GetHeight() - 1}});

  InfoBoxFactory::Type type = parent->GetContents(i);
  const TCHAR *caption = type < InfoBoxFactory::NUM_TYPES
    ? InfoBoxFactory::GetCaption(type)
    : NULL;
  if (caption == NULL)
    caption = _("Invalid");
  else
    caption = gettext(caption);

  canvas.Select(parent->GetInfoBoxLook().title_font);
  canvas.SetBackgroundTransparent();
  canvas.SetTextColor(is_current ? COLOR_WHITE : COLOR_BLACK);
  canvas.DrawText({2, 2}, caption);
  */
}



std::unique_ptr<Widget>
CreateInfoBoxesConfigPanel()
{
  return std::make_unique<InfoBoxesConfigPanel>();
}

