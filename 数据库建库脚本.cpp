    QSqlDatabase db;
    db=QSqlDatabase::addDatabase("QSQLITE");//建库
    db.setDatabaseName("air_conditioner.db");//命名库

    /*分别建Query，Windspeed，ID，Shutdown表*/
    QSqlQuery query;
     query.prepare("create table if not exists Query(queryid varchar primary key,roomid varchar,start_time varchar,end_time varchar,start_temp float,end_temp float,cost float)");
     query.prepare("create table if not exists Windspeed(queryid varchar,start_time varchar,end_time varchar,windspeed int)");
     query.prepare("create table if not exists ID(roomid varchar,idnum varchar)");
      query.prepare("create table if not exists Shutdown(roomid varchar,time varchar)");