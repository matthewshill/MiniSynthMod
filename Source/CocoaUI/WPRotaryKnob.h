//
//  WPRotaryKnob.h
//
//  Created by Matt Hill
//  Adapted from the MiniSynth diagram from Will Pirkle
//
/*
 
 */

#import <AppKit/AppKit.h>

/************************************************************************************************************/
/* NOTE: It is important to rename ALL ui classes when using the XCode Audio Unit with Cocoa View template	*/
/*		 Cocoa has a flat namespace, and if you use the default filenames, it is possible that you will		*/
/*		 get a namespace collision with classes from the cocoa view of a previously loaded audio unit.		*/
/*		 We recommend that you use a unique prefix that includes the manufacturer name and unit name on		*/
/*		 all objective-C source files. You may use an underscore in your name, but please refrain from		*/
/*		 starting your class name with an undescore as these names are reserved for Apple.					*/
/************************************************************************************************************/
@interface WPEditBoxMS : NSTextField
{
    bool editing;
    id target;
    SEL editStartedAction;
    SEL editEndedAction;
}
-(bool)isEditing;
-(void)setTarget:(id)control editStartedAction:(SEL)sel1 editEndedAction:(SEL)sel2;
@end

// --- our custom Cell 
@interface WPRotaryKnobCell : NSActionCell 
@end

// --- note rename to <control>DX; see above from Apple
@interface WPRotaryKnobMS : NSControl
{
    // --- these are raw positions -1 to +1 ordinarily
    CGSize currentPosition;  // current position
    CGSize mouseDownPosition; // position of knob when mousedown
    
    // --- mouse down location
    CGSize mouseDownLocation;

    // --- default is 1.0
    float knobScale;

    // --- array for knob images
    NSMutableArray* knobImages;
    
    // --- min, max and init
    float minValue;
    float maxValue;
    float defaultValue;
    
    // --- control Name
    NSTextField* knobTextField;
    NSString* controlName;
    
    // --- control edit box
    WPEditBoxMS* controlEditBox;
    
    // --- the ControlID for this group (optional, for WP Synth stuff)
    int controlID;
    
    // --- volt/octave scaling
    bool voltPerOctaveControl;
    
    // --- integer-based
    bool integerControl;
    
    // --- to hide the edit control
    bool hideEditBox;
    
    // --- mini-knob
    bool miniKnob;
}

// --- this sets up the value range and init value
- (void)initControlWithName:(NSString*)name 
               controlIndex:(int)index 
                        min:(float)minV 
                        max:(float)maxV 
                        def:(float)defaultV 
                 voltOctave:(bool)vpo 
             integerControl:(bool)intControl;

// --- methods to set knob attributes; note the knob image array is hard-coded
//     in the initializer
-(void)setKnobImageArray:(NSMutableArray*) imageArray;

// --- can scale to a new range of [-scale..+scale]
//     but I do not use this, instead use initValuesWithMin:andMax:andDefault
- (float)scale;
- (void)setKnobScale:(float)knobScale;

// --- edit box background is transparent by default (looks a bit cleaner)
- (void)setTransparentEditBoxBackground:(bool)transparent;

// --- must use 1/2 sized View containers
- (void)setMiniKnob:(bool)mini;

// --- knob/title only (make title = "" for empty)
- (void)hideEditBox:(bool)hide;

// --- get our ID value (or -1 if not set)
- (int)controlID;

// --- value = user range of values
- (float)controlValue; // user values
- (void)setControlValue:(float)userValue; // user values

// --- for fc and other frequency based knobs
- (void)setVoltPerOctaveControl:(bool)vpo;

// --- action methods for the buddy-edit control
- (void)editBoxEndEdit:(id)sender;
- (void)editBoxStartEdit:(id)sender;

@end





