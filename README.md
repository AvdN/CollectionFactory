CollectionFactory
=================

[![Build Status](https://travis-ci.org/AvdN/CollectionFactory.svg?branch=master)](https://travis-ci.org/AvdN/CollectionFactory)

Translation between native collections in Objective-C and serialized formats
like JSON.

Static methods always return `nil` if an error occurs (such as JSON could not be
passed, was nil, or was an invalid expected type).


* [Converting to JSON](#converting-to-json)
* [Converting from JSON](#converting-from-json)
* [Creating Mutable Objects](#creating-mutable-objects)
* [Loading from Files](#loading-from-files)


Converting to JSON
------------------

### Native Types

You can use `jsonString` or `jsonData` to get the NSString or NSData encoded
versions in JSON respectively.

```objc
NSDictionary *d = @{@"foo": @"bar"};

// {"foo":"bar"}
NSString *jsonString = [d jsonString];

// The same value as above but as a NSData
NSData *jsonData = [d jsonData];
```

Both methods are available on `NSNull`, `NSNumber`, `NSArray`, `NSDictionary`,
`NSObject`, and `NSString`.

### Custom Types

You may also convert any subclass of `NSObject`:

```objc
@interface SomeObject : NSObject

@property NSString *string;
@property int number;

@end
```

```objc
SomeObject *myObject = [SomeObject new];
myObject.string = @"foo";
myObject.number = 123;

// {"string":"foo","number":123}
NSString *json = [myObject jsonString];
```

If you need to control how custom objects are serialized you may override the
`[jsonDictionary]` method:

```objc
@implementation SomeObject

- (NSDictionary *)jsonDictionary
{
    return @{
        @"number": self.number,
        @"secret": @"bar",
    };
}

@end
```

```objc
SomeObject *myObject = [SomeObject new];
myObject.string = @"foo";
myObject.number = 123;

// {"number":123,"secret":"bar"}
NSString *json = [myObject jsonString];
```

Converting from JSON
--------------------

### Native Types

The simplest way to convert JSON to an object is to run it through NSObject:

```objc
NSString *json = @"{\"foo\":\"bar\"}";
id object = [NSObject objectWithJsonString:json];
```

However, if you know the type of the incoming value you should use the
respective class factory (rather than blindly casting):

```objc
NSString *json = @"{\"foo\":\"bar\"}";
NSDictionary *d = [NSDictionary dictionaryWithJsonString:json];
```

When using a specific class it will not accept a valid JSON value of an
unexpected type to prevent bugs occuring, for example:

```objc
NSString *json = @"{\"foo\":\"bar\"}";

// `a` is `nil` because we only intend to decode a JSON array.
NSArray *a = [NSArray arrayWithJsonString:json];

// `b` is an instance of `NSDictionary` but future code will be treating it like
// an `NSArray` which will surely cause very bad things to happen...
NSArray *b = [NSObject objectWithJsonString:json];
```

### Custom Types

Let's say you have this:

```objc
@interface SomeObject : NSObject

@property NSString *string;
@property int number;

@end
```

The same method that unwraps native types is used except because the static
method `[objectWithJsonString:]` is called against `SomeObject` you are saying
that it must unserialize to that type of object.

```objc
NSString *json = @"{\"string\":\"foo\",\"number\":123};

SomeObject *myObject = [SomeObject objectWithJsonString:json];

// 123
myObject.number;

// Do NOT do this. Otherwise you will get an NSDictionary.
// SomeObject *myObject = [NSObject objectWithJsonString:json];
```

Objects are contructed recursively by first checking to see if the property
exists, if it does and the data is not prefixed with `NS` it will create another
custom object and continue. This means JSON can be used to unpack simple objects
without any specific code however this has some caveats:

  1. It is dangerous. Not all properties are public or even exist so types can
     be easily missing and cause serious memory error when trying to use the
     unpacked objects.

  2. It will always use the `[init]` constructor which may be wrong or not even
     available making the object constructions impossible.

A much safer way to unpack objects is to override the `[setValue:forProperty:]`
method. This allows you to control exactly what logic you need with each
property.

Note: This is is a wrapper for `[setValue:forKey:]` and will call
`[setValue:forKey:]` if you have not overridden it.

```objc
@implementation SomeObject

- (void)setValue:(id)value forProperty:(NSString *)key
{
    // Only allow these two properties to be set.
    NSArray *properties = @[@"number", @"string"];
    if ([properties indexOfObject:key] != NSNotFound) {
        [self setValue:value forKey:key];
    }
}

@end
```

Creating Mutable Objects
------------------------

For every factory method there is a mutable counterpart used for generating
objects that be safely editly directly after unpacking.

```objc
NSString *json = @"{\"foo\":\"bar\"}";
NSDictionary *d = [NSDictionary dictionaryWithJsonString:json];
NSMutableDictionary *md = [NSMutableDictionary mutableDictionaryWithJsonString:json];
```

Loading from Files
------------------

Each factory method also has a way to generate the object directly from a file:

```objc
NSArray *foo = [NSArray arrayWithJsonFile:@"foo.json"];
```

If the file does not exist, there was an error parsing or the JSON was the wrong
type then `nil` will be returned.

Futhermore you can create mutable objects from files:

```objc
NSMutableArray *foo = [NSMutableArray mutableArrayWithJsonFile:@"foo.json"];
```
