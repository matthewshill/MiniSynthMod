//
//  WPOptionMenuGroup.h
//
//  Created by Matt Hill
//  Adapted from the MiniSynth diagram from Will Pirkle
//
/*
 
 */

#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>
@interface WPPopUpButtonCellMS : NSPopUpButtonCell
{
    bool menuShowing;
    bool verySmallFont;
}
-(void)setVerySmallFont:(bool)smallFont;
@end

@interface WPOptionMenuGroupMS : NSControl
{
    // --- control name
    NSTextField* controlTextField;
    
    // --- name string
    NSString* controlName;
    
    // --- string of substrings, separated by commas eg: "LPF,HPF,BPF"
    NSString* enumString;
   
    // --- the ControlID for this group (optional, for WP Synth stuff)
    int controlID;
    
    // --- current selection
    int selectedIndex;

    // --- our pop-up menu
    NSPopUpButton* popUpButton;    
}

// --- this sets up the value range and init value
- (void)initControlWithName:(NSString*)name 
               controlIndex:(int)index 
                 enumString:(NSString*)enumStr 
                        def:(float)defaultV 
              verySmallFont:(bool)smallFont;

// --- action for menu buddy
- (void)menuItemChanged:(id)sender;

// --- get the current selection
- (float)currentSelection;

// --- set the current selection; note this function has the 
//     same name in the WPKnobControl and therefore is used
//     in a "double" way (see the view object) In this case
//     the userValue is cast to an (int) and is the selection
- (void)setControlValue:(float)userValue; // user values

- (int)controlID;

@end
