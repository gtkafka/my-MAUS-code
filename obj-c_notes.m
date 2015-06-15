INSTANCE METHODS

(int)age;
(void)setAge:(int)age; //use colon

(void)init; //initialize object. alloc -> "new" in java
(void)initWithName:(NSString *)name andAge:(int)age; 
//called "init with name and age" pointer to a sting object. andAge is the second parameter.

//getters and setters are made for you.

student.m

@implementation Student
- (int)age
{
  return _age;
}

- (void)setAge:(int)age
{
  _age = age;
}

- (NSString *)name
{
  return _name;
}

-(void)setName:(NSString *)name
{
  _name = [name copy];//copy the bits the 
}

@end

//generates getters and setters:
@implementation Student
@synthesize age=_age;
@synthesize name=_name;
@end

int main(int argc, const char * argv[])
{
  @autoreleasepool {
    Student *alice = [Student alloc];
    alice->age = 20;
    //or    
    [alice setAge:20]; //sends a message
    alice.age = 20; //call a method "setAge"
    alice->name = @"Alice"; 
    Student *alice = [[Student alloc] initWithName:@"Alice" andAge:20];
    //asigning name instance variable. like alice.name 
    //or "(*alice).age" or "go to alice and get age" 
    greet(alice); 
  }
  return 0;
}
void greet(Student *s)
{
  NSLog(@"Hello, %@.", s->name);
  //or
  NSLog(@"Hellos, %@.", [s name]);
  NSLog(@"Hellos, %@.", s.name);
}

student.h

@interface Student : NSObject {
  int _age;
  NSString *_name;
}
//enables "dot" notation! (this is all that is needed) and sets getters and setters
@property (assign, nonatomic, readwrite) int age;
@property (copy, nonatomic, readwrite) NSString *name;

- (id)initWithName:(NSString *)name andAge:(int)age;//not declaring alloc (it is inherited)

//getters and setters
- (int)age;
- (void)setAge:(int)age;

- (NSString *)name;
- (void)setName:(NSString *)name;
@end 

//OBJECTIVE-C 2.0

- (void)setName:(NSString *)name
{
  if ([name isEqualToString:@"David"]) {
    _name = [[NSString alloc] initWithString:@"Dummy"]; //allocates enough space for "Dummy" 
  }
  else {
    _name = [name copy];
  }
}

@end


//student.m

@implementation Student

@synthesize age=_age;
@synthesize name=_name;

- (id)init
{
  self = [self initWithName:@"John" andAge:404];
  return self;
}

- (id)initWithName:(NSString *)name andAge:(int)age //id is like void*
{
  if (self = [super init])
    {
      self.age = age;
      self.name = name;
    }
  return self;
}

int main(int argc, const char * argv[])
{
  @autoreleasepool {
    NSMutableArray *students = [[NSMutableArray alloc] init];
    [students addObject:[[Student alloc] initWithName:@"Alice";]]
    for (Student *s in students {
	  greet(s);
      }

	 
   
