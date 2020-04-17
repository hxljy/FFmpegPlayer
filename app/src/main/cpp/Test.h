//
// Created by 刘德利 on 2020-02-10.
//

#ifndef KEVINPLAYER_TEST_H
#define KEVINPLAYER_TEST_H


class Test {
public:
    Test();

    // 自动 给 析构函数 增加的 虚函数标记，其实不需要
    // 如果我的这个Test类有之类，就必须增加虚函数
    /*virtual*/ ~Test();
};


#endif //KEVINPLAYER_TEST_H
