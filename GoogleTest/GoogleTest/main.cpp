
//#include <QtCore/QCoreApplication>
#include <QApplication>
#include <gtest/gtest.h>
#include <QFileDialog>

int Add(int a, int b)
{
	return a + b;
}

TEST(AddTestCase, AddTest)
{
	EXPECT_EQ(5, Add(2, 3));
	EXPECT_EQ(6, Add(3, 3));
	EXPECT_EQ(7, Add(1, 6));
	EXPECT_EQ(8, Add(3, 5));
}

int main(int argc, char *argv[])
{
	//QCoreApplication a(argc, argv);
	QApplication a(argc, argv);

	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	RUN_ALL_TESTS();

	return a.exec();
}
