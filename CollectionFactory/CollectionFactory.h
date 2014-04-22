@interface CollectionFactory : NSObject

+ (NSArray *)arrayWithJsonString:(NSString *)rawJson;
+ (NSArray *)arrayWithJsonData:(NSData *)rawJson;
+ (NSDictionary *)dictionaryWithObject:(id)object;
+ (NSString *)jsonStringWithArray:(NSArray *)array;
+ (NSDictionary *)dictionaryWithJsonData:(NSData *)rawJson;
+ (id)parseWithJsonData:(NSData *)rawJson options:(NSJSONReadingOptions)options mustBeOfSubclass:(Class)class;

@end
