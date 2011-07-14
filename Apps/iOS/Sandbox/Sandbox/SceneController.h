//
//  SceneController.h
//

#import <Foundation/Foundation.h>


struct SceneControllerImpl;

@class ESRenderView;

@interface SceneController : NSObject {
@private
  
  struct SceneControllerImpl* Internal;
}

@property (nonatomic, retain) ESRenderView* renderView;

-(void)initializeScene;
-(void)resetView;

@end
