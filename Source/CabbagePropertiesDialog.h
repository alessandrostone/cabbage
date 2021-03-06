#ifndef __CB_PROPS__
#define __CB_PROPS__
/*
  Copyright (C) 2007 Rory Walsh

  Cabbage is free software; you can redistribute it
  and/or modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  Cabbage is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with Csound; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  02111-1307 USA

*/
#include "../JuceLibraryCode/JuceHeader.h"
#include "CabbageCallOutBox.h"
#include "CabbageLookAndFeel.h"
#include "CabbageGUIClass.h"
#include "./Plugin/CabbagePluginProcessor.h"


class ColourField;
class TextComboField;
class TextField;
class FileBrowserField;
class colourPallete;

extern CabbageLookAndFeel* lookAndFeel;
extern CabbageLookAndFeelBasic* lookAndFeelBasic;
extern Array <Colour> swatchColours;

//============================================================
// classes for displaying widget properties
//============================================================
class ControlProperty : public PropertyComponent,
    public ActionListener
{
public:
    //constructor for ControlProperty
    ControlProperty(String name, var value);

    ~ControlProperty();

    void paint(Graphics &g);
    void resized();
    void refresh()
    {

    }

    void actionListenerCallback (const String& message);
    ScopedPointer<TextField> textField;
    ScopedPointer<TextComboField> textComboField;
    ScopedPointer<ColourField> colourField;
    ScopedPointer<FileBrowserField> fileBrowserField;

    String name;
    var value;
};


class CabbagePropertiesPanel : public PropertyPanel,
    public ChangeBroadcaster
{
public:
    CabbagePropertiesPanel(String name) : PropertyPanel(name)
    {
        setSize(200, 500);
        //juceLookAndFeel = new LookAndFeel_V1();
        //setLookAndFeel(juceLookAndFeel);
    }



    void paint(Graphics &g)
    {
        g.setColour(Colours::white);
#ifdef CABBAGE_HOST
        g.drawFittedText("Press ESC to update widget parameters", 0, getHeight()-25, getWidth(), 16, Justification::centred, 1, 1);
#endif
    }

    ///Update properties based on state of Cabbage widget
    void updateProperties(CabbageGUIClass &cAttr)
    {
        attributeNames.clear();
        textFields.remove(0);
        comps.clear();
        int hiddenComponents = 0;

        for(int i=0; i<cAttr.cabbageIdentifiers.size(); i++)
        {
            attributeNames.add(cAttr.cabbageIdentifiers.getName(i).toString());
            textFields.append(Value(cAttr.cabbageIdentifiers.getValueAt(i)));
            comps.add(new ControlProperty(attributeNames[i], textFields[i]));
            comps[i]->setPreferredHeight(22);


            if(attributeNames[i].equalsIgnoreCase("type") ||
                    attributeNames[i].equalsIgnoreCase("name") ||
                    attributeNames[i].equalsIgnoreCase("caption") ||
                    attributeNames[i].equalsIgnoreCase("kind") ||
                    attributeNames[i].equalsIgnoreCase("scaley") ||
                    attributeNames[i].equalsIgnoreCase("scalex") ||
                    attributeNames[i].equalsIgnoreCase("range") ||
                    attributeNames[i].equalsIgnoreCase("channelarray") ||
                    attributeNames[i].equalsIgnoreCase("offfontcolour") ||
                    attributeNames[i].equalsIgnoreCase("popup") ||
                    attributeNames[i].equalsIgnoreCase("comborange") ||
                    attributeNames[i].equalsIgnoreCase("basetype") ||
                    attributeNames[i].equalsIgnoreCase("rangey") ||
                    attributeNames[i].equalsIgnoreCase("rangex") ||
                    attributeNames[i].equalsIgnoreCase("widgetarray") ||
                    attributeNames[i].equalsIgnoreCase("xyautoindex") ||
                    attributeNames[i].equalsIgnoreCase("decimalplaces"))
            {
                //hide these identifiers from the properties component
                comps[i]->setPreferredHeight(0);
                hiddenComponents++;
            }

        }

        //adding 30 to show update text message
        setSize(200, (30+(comps.size()-hiddenComponents)*22)+22);
        Logger::writeToLog(String(this->getHeight()));
        addProperties(comps);
        repaint();

    }

    int getNumberOfProps()
    {
        return comps.size();
    }

    String getPropName(int index)
    {
        ControlProperty* prop = dynamic_cast<ControlProperty*>(comps[index]);
        if(prop)
            return prop->name;
        else return "shouldn't happen";
    }

    var getPropValue(int index)
    {
        ControlProperty* prop = dynamic_cast<ControlProperty*>(comps[index]);
        if(prop)
            return prop->value;
        else return "shouldn't happen";
    }

    ~CabbagePropertiesPanel()
    {
        comps.clear();
        //juceLookAndFeel = nullptr;
        Logger::writeToLog("update all variables before closing");
    }

    var textFields;
    Array <PropertyComponent*> comps;
    StringArray attributeNames;
    String myText;
    //ScopedPointer<LookAndFeel_V1> juceLookAndFeel;

};

class colourPallete : public ColourSelector
{
public:
    colourPallete(): ColourSelector()
    {
        setColour(ColourSelector::backgroundColourId, Colours::black);
        setColour(ColourSelector::labelTextColourId, Colours::white);
    };

    ~colourPallete() {};


    int getNumSwatches() const
    {
        return swatchColours.size();
    }

    Colour getSwatchColour(int index) const
    {
        return swatchColours[index];
    }

    void setSwatchColour (const int index, const Colour &newColour) const
    {
        swatchColours.set(index, newColour);
    }

    String getNameOfParent()
    {
        return nameOfParent;
    }

    void setNameOfParent(String parent)
    {
        nameOfParent = parent;
    }

private:
    String nameOfParent;

};

class CabbagePropertiesDialog : public DocumentWindow,
    public ActionBroadcaster
{
public:
    CabbagePropertiesDialog(String name):DocumentWindow ("Properties", Colour(20, 20, 20),
                DocumentWindow::minimiseButton
                | DocumentWindow::closeButton),
        cAttr("", -99)
    {
        //setSize(500, 300);
        lookAndFeel = new CabbageLookAndFeel();
        propsPanel = new CabbagePropertiesPanel("");
        propsPanel->setLookAndFeel(lookAndFeel);
        setSize(200, propsPanel->getHeight());
#ifndef LINUX
        setAlpha(.9f);
#endif
    }

    ~CabbagePropertiesDialog()
    {
        propsPanel=nullptr;
    }


    void closeButtonPressed()
    {
        setVisible(false);
    }

    void updateProps(CabbageGUIClass &cAttr)
    {
#ifndef CABBAGE_HOST
        propsPanel = nullptr;
        propsPanel = new CabbagePropertiesPanel("");

        propsPanel->updateProperties(cAttr);
        setContentNonOwned(nullptr, false);
        setContentNonOwned(propsPanel, true);
#endif
    }

    void updatePropertyPanel(CabbagePropertiesPanel* panel)
    {
        if(panel!=nullptr)
        {
            propsPanel = panel;
        }
    }

    void updateIdentifiers()
    {
        updatedIdentifiers.clear();
        for(int i=0; i<propsPanel->getNumberOfProps(); i++)
        {
            Logger::writeToLog("CabbagePropertiesDialog::keyPressed()");
            Logger::writeToLog(propsPanel->getPropName(i));
            Logger::writeToLog(propsPanel->getPropValue(i).toString());
            updatedIdentifiers.set(propsPanel->getPropName(i), propsPanel->getPropValue(i));
        }
        sendActionMessage("Message sent from PropertiesDialog");
    }

    bool keyPressed(const KeyPress &key)
    {
        return false;
    }


    void paint(Graphics& g)
    {
        g.fillAll(Colours::black);
        g.setColour(Colour(40, 40, 40));
        //g.fillRoundedRectangle(10,  getHeight()-26, getWidth()-20, 20, 2);
        g.setColour(Colours::white);
        g.drawFittedText("Press ESC to update", 0, getHeight()-25, getWidth(), 16, Justification::centred, 1, 1);
    }

    CabbagePropertiesPanel* propsPanel;
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;
    NamedValueSet updatedIdentifiers;
    CabbageGUIClass cAttr;
};

class ColourField : public Component,
    public ChangeListener,
    public ActionBroadcaster
{

public :

    ColourField(String name, String string):name(name), Component("ColourField")
    {
        colour = Colour::fromString(string);
        value.append(colour.getRed());
        value.append(colour.getGreen());
        value.append(colour.getBlue());
        value.append(colour.getAlpha());
        sendActionMessage("ColourField");
        setSize(100, 30);
    }

    ~ColourField()
    {

    }



    void paint(Graphics &g)
    {
        g.fillAll(Colours::black);
        g.setColour(cUtils::getBorderColour());
        float borderWidth = cUtils::getBorderWidth();
        g.fillRoundedRectangle(0.f, 0.f, getWidth()-borderWidth, getHeight()-borderWidth,  getHeight()*0.1);
        g.setColour(colour);
        g.fillRoundedRectangle(2.f, 2.f, getWidth()-borderWidth-2, getHeight()-borderWidth-2,  getHeight()*0.1);
        g.setColour (colour.contrasting());
        g.setFont (cUtils::getComponentFont());
        const juce::Rectangle<int> r (5, 0, getWidth()-1, getHeight()+1);
        g.drawFittedText(colour.toString(), r, Justification::centred, 2);
    }

    void mouseDown(const MouseEvent& e)
    {

        colourSelector.setBounds(0, 0, 300, 300);
        colourSelector.addChangeListener(this);
        colourSelector.setNameOfParent(name);
        colourSelector.setCurrentColour(colour);
        juce::Rectangle<int> rect;
        CabbageCallOutBox callOut (colourSelector, rect, nullptr);
        callOut.setLookAndFeel(lookAndFeelBasic);
        callOut.setTopLeftPosition(this->getScreenX()+this->getWidth(), e.getScreenY()-this->getHeight());
        callOut.setAlwaysOnTop(true);

#if !defined(AndroidBuild)
        callOut.runModalLoop();
#endif
        colour = colourSelector.getCurrentColour();
        value.resize(0);
        value.append(colour.getRed());
        value.append(colour.getGreen());
        value.append(colour.getBlue());
        value.append(colour.getAlpha());
        sendActionMessage("ColourField");
        sendActionMessage("UpdateAll");
        this->getTopLevelComponent()->grabKeyboardFocus();

    }

    void changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        colourPallete* cs = dynamic_cast <colourPallete*> (source);
        if(cs->getNameOfParent()==name)
            colour = cs->getCurrentColour();
        repaint();
        value.resize(0);
        value.append(colour.getRed());
        value.append(colour.getGreen());
        value.append(colour.getBlue());
        value.append(colour.getAlpha());
    };


    void resized()
    {
    }


    Colour colour;
    String rgbaString;
    var value;
    String name;
    colourPallete colourSelector;

};


class TextField : public TextEditor,
    public ChangeBroadcaster,
    public ActionBroadcaster,
    public Timer
{
public:
    TextField(String name):TextEditor(""),
        name(name)
    {
        setFont(Font("Arial", 14, 1));
        setColour(TextEditor::textColourId, Colours::whitesmoke);
        setColour(CaretComponent::caretColourId, Colours::white);
        setColour(TextEditor::backgroundColourId, Colour(20, 20, 20));
        setColour(TextEditor::highlightColourId, Colours::cornflowerblue);
        setColour(TextEditor::highlightedTextColourId, Colours::white);
        setColour(TextEditor::outlineColourId, Colours::black);
#ifndef CABBAGE_HOST
        setColour(TextEditor::focusedOutlineColourId, Colours::orange);
#else
        setColour(TextEditor::focusedOutlineColourId, Colours::whitesmoke);
#endif
    }

    ~TextField()
    {
        removeAllActionListeners();
        removeAllChangeListeners();
    }


    bool getWantsKeyboardFocus()
    {
        return true;
    }

    void paint(Graphics &g)
    {
        g.fillAll(Colours::black);
    }

    void updateText()
    {
        StringArray array;
        value = "";
        value.resize(0);
        array.addLines(this->getText());
        Logger::writeToLog(this->getText());
        if(array.size()>1)
            for(int i=0; i<array.size(); i++)
                value.append(array[i]);
        else if(name!="value" || name!="min" || name!="max" || name!="midichan" ||
                name!="midictrl" || name!="top" || name!="tabbed" || name!="top" ||
                name!="textbox" || name!="popup")
        {
            value = this->getText();
        }
        else
            value = this->getText().getFloatValue();
    }


    bool keyPressed(const KeyPress &key)
    {
        if (key.getTextDescription().contains("cursor up"))
            moveKeyboardFocusToSibling(false);
        else if (key.getTextDescription().contains("cursor down"))
            moveKeyboardFocusToSibling(true);

        if (! TextEditorKeyMapper<TextEditor>::invokeKeyFunction (*this, key))
        {
            if(key == KeyPress::escapeKey)
            {
                updateText();
                //sendChangeMessage();
                sendActionMessage("TextField");
                sendActionMessage("UpdateAll");
                return false;

            }
            else
            {
                insertTextAtCaret (String::charToString (key.getTextCharacter()));
                updateText();
                sendActionMessage("TextField");

                return false;
            }
        }

        else
        {
            updateText();
            sendActionMessage("TextField");
            return true;
        }

    }

    void mouseDown(const MouseEvent& event)
    {
        startTimer(100);
    }


    void timerCallback()
    {
        stopTimer();
        grabKeyboardFocus();
    }

    var value;
    String name;
};


class FileBrowserField : public Component,
    public ActionBroadcaster,
    public ActionListener
{
    WildcardFileFilter wildcardFilter;
    ScopedPointer<CabbageLookAndFeel> lookAndFeel;

public :

    FileBrowserField(String name, String string):name(name), Component("FileBrowserField"),
        wildcardFilter("*", "*", "File filter"),
        lookAndFeel(new CabbageLookAndFeel())
    {
        addAndMakeVisible(textField = new TextField(name));
        textField->addActionListener(this);
        textField->setText(string);
        textField->setInterceptsMouseClicks(false, false);
        sendActionMessage("FileBrowserField");
        setSize(100, 30);
    }

    ~FileBrowserField()
    {

    }

    void mouseDown(const MouseEvent& e)
    {
#if !defined(AndroidBuild)

        //FileChooser openFC(String("Open a file..."), File::nonexistent, String("*.*"), UseNativeDialogue);

        if(!e.mods.isCtrlDown())
        {
            //wildcardFilter = WildcardFileFilter("*", directory.getFullPathName(), "File fitler");

            Array<File> selectedFiles = cUtils::launchFileBrowser("Select a file",
                                        wildcardFilter,
                                        "",
                                        1,
                                        File(""),
                                        true,
                                        lookAndFeel);
            if(selectedFiles.size()>0)
            {
                value.resize(0);
                //add file name to value
                value.append(selectedFiles[0].getFullPathName());
                sendActionMessage("FileBrowserField");
                sendActionMessage("UpdateAll");
                textField->setText(value[0]);
                this->getTopLevelComponent()->grabKeyboardFocus();
            }

        }
#endif
    }


    void actionListenerCallback (const String& message)
    {
        if(message=="UpdateAll")
        {
            value.resize(0);
            value.append(textField->getText());
            sendActionMessage("FileBrowserField");
            sendActionMessage("UpdateAll");
        }
    }

    void resized()
    {
        textField->setBounds(0, 0, getWidth(), getHeight());
    }

    var value;
    String name;
    ScopedPointer<TextField> textField;
};

class TextComboField : public Component,
    public ChangeListener,
    public ChangeBroadcaster,
    public ActionBroadcaster
{
public:
    TextComboField(String name, var _value):
        Component(),
        value(_value),
        textEditor(name)
    {
    }

    ~TextComboField()
    {

    }

    void paint(Graphics &g)
    {
        g.setColour (Colours::white);
        g.setFont (cUtils::getComponentFont());
        const juce::Rectangle<int> r (5, 0, getWidth(), getHeight());

        if(value.size()>0)
            g.drawFittedText(value[0].toString(), r, Justification::centredLeft, 2);
        else
            g.drawFittedText(value.toString(), r, Justification::centredLeft, 2);
    }

    void mouseDown(const MouseEvent& e)
    {
        text="";
        textEditor.setFont(Font("Arial", 14, 1));
        textEditor.addChangeListener(this);

        textEditor.setColour(TextEditor::textColourId, Colours::white);
        textEditor.setColour(TextEditor::backgroundColourId, Colour(20, 20, 20));
        textEditor.setColour(TextEditor::highlightColourId, Colours::cornflowerblue);
        textEditor.setColour(TextEditor::highlightedTextColourId, Colours::white);
        textEditor.setColour(TextEditor::outlineColourId, Colours::white);
        textEditor.setMultiLine(true);

        if(value.size()==0)
            text = value.toString();
        else
            for(int i=0; i<value.size(); i++)
            {
                text << value[i].toString() << "\n";
            }

        textEditor.setText(text.trim());
        textEditor.setBounds(0, 102, 150, 70);
        juce::Rectangle<int> rectum(0, 100, 100, 70);
        CabbageCallOutBox callOut (textEditor, rectum, nullptr);
        callOut.setLookAndFeel(lookAndFeelBasic);
        callOut.setTopLeftPosition(this->getScreenX()+this->getWidth(), e.getScreenY()-150/2);
        callOut.setAlwaysOnTop(true);
#if !defined(AndroidBuild)
        callOut.runModalLoop();
#endif
        value = textEditor.value;
        sendActionMessage("TextComboField");
        sendActionMessage("UpdateAll");
        this->repaint();
        this->getTopLevelComponent()->grabKeyboardFocus();

    }

    void changeListenerCallback(juce::ChangeBroadcaster *source)
    {
        TextField* cs = dynamic_cast <TextField*> (source);
        value = cs->value;
        this->repaint();
        sendChangeMessage();
    };

    TextField textEditor;
    var value;
    String text;
};



#endif