#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

/******************************************************************
 *  Sql test (works)
 *****************************************************************/
void DemoSql()
{
	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("PlayGoDataBase.db3");
	db.open();

	QSqlQuery q("", db);
//		q.exec("drop table Movies");
	q.exec("drop table Names");
	q.exec("drop table groceries");
//		q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
//		q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
//		q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
//		q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
//		q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
//		q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
	q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
	q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
	q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
	q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
	q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
	q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
	q.exec("insert into Names values (9, 'Buck', 'Rogers', 'Paris')");
	q.exec("insert into Names values (19, 'Sherlock', 'Holmes', 'London')");
	q.exec("insert into Names values (7, 'Leonardo', 'Caprio', 'America')");
	q.exec("insert into Names values (8, 'No Oscar', 'Caprio', 'America')");
	q.exec("create table groceries (id integer primary key, name varchar, quantity number, price number)");
	q.exec("insert into groceries values (1, 'banana', '10', '1.56')");
	q.exec("insert into groceries values (2, 'apple', '14', '4.56')");
	db.close();
}