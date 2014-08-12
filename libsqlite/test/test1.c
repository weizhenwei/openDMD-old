#include <stdio.h>
#include <sqlite3.h>
 
int main(void)
{
    /*定义一个数据库连接对象指针*/
    sqlite3 *db = NULL;

    /*初始化连接对象开辟空间*/
    int rc = sqlite3_open("sqlite.db", &db);
    if(rc != SQLITE_OK)
    {
        /*获取连接对象的错误信息*/
        fprintf(stderr,"%s\n",sqlite3_errmsg(db));
        return -1;
    }
    printf("connect sucess!\n");
 
    sqlite3_close(db);   //闭关数据库
 
    return 0;
}
