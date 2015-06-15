//iOS programming
//app launch cycle
//user taps app icon -> main() -> UIAPPlication() -> Load man UI file 
//->initialize the app -> activate the app -> EVENT LOOP

//UIKit
//UIApplicationDelegate
//UIView
//UIViewController controls what user sees on screen.
//UIWindow
//VIEW <-> CONTROLLER <-> MODEL

V.7
int main(int argc, const char * argv[])
{
  @autoreleasepool {
    //creates a pointer to an object of type NSArray. must include nil
    NSArray *names = [NSArray arrayWithObjects:@"Alice", @"Bob", nil];
    for (NSString *name in names) {
      NSLog(@"Hello, %@.", name);
    }
  }
  return 0;
}

V.8
int main(int argc, const char *argv[])
{
  @autoreleasepool {
    //creates a pointer to an object of type NSArray. must include nil                                
    NSMutableArray *students = [NSMutableArray array];//64 bits
    NSMutableArray *students = [[NSMutableArray alloc] init];
    //same. using alloc method and then calling init
    
    Student *alice = [Student studentWithName:@"Alice" andAge:20];
    [students addObject:alice];
    Student *alice = [Student studentWithName:@"Bob" andAge:20];
    [students addObject:bob];
    for (NSString *namein names) {
      NSLog(@"Hello, %@.", [student.name]);
    }
  }
  return 0;
}

V.9
@interface Student : NSObject

//enables "dot" notation! (this is all that is needed) and sets getters and setters                   
@property (assign, nonatomic, readwrite) int age;
@property (copy, nonatomic, readwrite) NSString *name;

- (id)initWithName:(NSString *)name andAge:(int)age;//not declaring alloc (it is inherited)
- (void)greet;

@end

@implementationStudent

@synthesize age=_age;
@synthesize name=_name;

- (id)init
{
  self = [self initWithName:@"John" andAge:404];
  return self;
}

- (id)initWithName:(NSString *)name andAge:(int)age //id is like void*                                
{
  if (self = [super init])//parent class of init (NSObject). an explicit constructor
    {
      self.age = age;
      self.name = name;
    }
  return self;
}

- (void)greet
{
  NSLog(@"Hello, I am %@. I am %d years old.\n", self.name, self.age);//invoking get from @synth
} 

int main(int argc, const char * argv[])
{
  @autoreleasepool {
    Student *alice = [Student studentWithName:@"Alice" andAge:20];
    Student *alice = [Student studentWithName:@"Bob" andAge:21];
    [alice greet];
    [bob greet];
  }
  return 0;
}

int x = 0;
int y = 1;
swap(&x, &y);

void swap(int *a, int *b)
{
  int tmp = *a;
  *a = *b;
  *b = tmp;
}
// if we did this w/o pointers, we "stack" 
//a swap in memory and then throw it away.
