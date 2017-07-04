//
//  ViewController.m
//  MCDemo
//
//  Created by Visionin on 17/7/3.
//  Copyright © 2017年 Rex. All rights reserved.
//

#import "ViewController.h"
#include "libzippp.h"
#include "McBasic.h"
#include "McZip.h"

using namespace mc;

class MyHttpCallback: public HttpCallback{
public:
    virtual void done(int http_code, status_t st, char* text){
        printf("http: %d, %d %s", http_code, st, text);
    }
};

@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    HttpSession http;
    MyHttpCallback* callback = new MyHttpCallback();
    http.get("http://www.baidu.com/", callback);
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
