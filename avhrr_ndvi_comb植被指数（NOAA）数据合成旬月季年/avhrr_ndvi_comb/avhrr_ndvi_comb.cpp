// avhrr_ndvi_comb.cpp : �������̨Ӧ�ó������ڵ㡣
//���NOAA AVHRR NDVI�����ݲ�Ʒ������Ѯ���¡�����12-2��3-5��6-8��9-11�����������ϡ�����㷨����ƽ������Сֵ�����ֵ��
//����Ѯ�Ŀ�ʼ����Ϊÿ���µĵ�һ�죬ÿ���µĵ�31�첻����ͳ�ƣ�2�·����һ�����Ѯ�м���ͳ�Ƽ��졣
//wangfeng1@piesat.cn 2017-9-25.
//0.1a 2017-9-30 ���ɺϳ����ݵ�ʱ����Ҫ�ж��Ƿ����Ŀ¼�Ѿ����˺ϳ����ݣ�������˾Ͳ�Ҫ�ظ�����
//0.2a 2017-9-30 ����Ŀ¼�����Ŀ¼��ͬһ��Ŀ¼ʱ������ֻ���avhrrԭʼ���ݽ������ϣ��ų��ϳ������ٴν��м��㡣
//0.3a 2017-10-9 ʹ��datacombinationfornoaandvi�������ݺϳɣ������˶�qa���жϡ�


#define UNICODE
#include "tinydir.h"
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <cmath>
#include "../../sharedcodes/wftools.h"
using namespace std;



void printCurrentTime() {
	//#include <ctime>
	time_t time0;
	time(&time0);
	std::cout << asctime(localtime(&time0)) << std::endl;
}




std::string getYmdFromAvhrrFilename(std::string filename)
{
	size_t posAvhrr = filename.rfind("AVHRR-Land");
	size_t pos1 = filename.rfind("_");
	size_t pos2 = filename.rfind(".nc");
	if (pos1 == std::string::npos || posAvhrr == std::string::npos || pos2 != filename.length()-3 )
	{
		return "";
	}
	size_t pos0 = pos1 - 8;
	std::string date0 = filename.substr(pos0, 8);
	return date0;
}



//����avhrr����
std::string lookingForAvhrrFilenameByDate8(std::vector<std::string>& filesVector ,  std::string dateYmd  )
{
	for (size_t i = 0; i < filesVector.size(); ++i)
	{
		// 	AVHRR-Land_v004_AVH13C1_NOAA-14_19950101_c20130923101030.nc
		size_t pos0 = filesVector[i].find("AVHRR-Land");
		size_t pos1 = filesVector[i].find("_AVH13C1_NOAA");
		size_t pos2 = filesVector[i].find(".nc");
		//bugfix 2017-9-30 only use avhrr orignal data file.
		if (pos0 != std::string::npos && pos1 != std::string::npos && pos2 == filesVector[i].length() - 3)
		{
			std::string ymd1 = getYmdFromAvhrrFilename(filesVector[i]);
			if (ymd1 == dateYmd) return filesVector[i];
		}
	}
	return "";
}







//��ȡAVHRR��Ʒ���
//AVHRR-Land_v004-preliminary_AVH13C1_NOAA-19_20170919_c20170920095925.nc
WYmd dateOfAvhrrFilename(std::string filename)
{
	WYmd ymd;
	ymd.year = -1;
	std::string ymdstr = getYmdFromAvhrrFilename(filename);
	if (ymdstr.length() == 8)
	{
		ymd.year = atof(ymdstr.substr(0, 4).c_str());
		ymd.month = atof(ymdstr.substr(4, 2).c_str());
		ymd.day = atof(ymdstr.substr(6, 2).c_str());
		ymd.isleapyear = wft_is_leapyear(ymd.year);
	}
	return ymd;
}




std::string makeAvhrrCombinationFilename(std::string filename0, std::string filename1 , std::string tail, string method )
{
	filename0 = wft_base_name(filename0);
	filename1 = wft_base_name(filename1);
	size_t pos1 = filename0.rfind("_");
	size_t pos0 = pos1 - 8;

	std::string prefix = filename0.substr(0, pos0);
	std::string date0 = getYmdFromAvhrrFilename(filename0);
	std::string date1 = getYmdFromAvhrrFilename(filename1);
	std::string outfname = prefix + date0 + "_" + method + "_" + tail;//2017-11-20.
	return outfname;
}

extern void processAllSeason(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination);
extern void processAllYear(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination);
extern void processAllMonth(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination);
extern void processAllXun(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination);
extern void processAllHou(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination);


//�������� 10d mon sea yea


int main(int argc, char** argv)
{
	std::cout << "Version 0.2a " << std::endl;
	std::cout << " v0.3a by wangfengdev@163.com 2017-10-9. use datacombinationfornoaandvi include QA filter." << std::endl;
	std::cout << " v0.4a by wangfengdev@163.com 2017-10-9. modify QA filter." << std::endl;
	std::cout << "v0.5a .2017-10-28 add type fiv/ten/sea/yea " << std::endl;
	std::cout << "v0.6a . parameters from startup.txt. add hou . change output name style donot use ymd_ymd style. use ymd_05d.tif like this. " << std::endl;
	if (argc == 1)
	{
		std::cout << "A program to generate bat file for avhrr combination." << std::endl;
		
		std::cout << " by wangfengdev@163.com 2017-9-25." << std::endl;
		
		std::cout << "Sample call:" << std::endl;
		std::cout << "  avhrr_ndvi_comb  -indir c:/datas/ -method max/aver -outdir c:/out/ [-type fiv/ten/sea/yea]  [-monthonly] -comb datacombinationfornoaandvi " << std::endl;
		cout << "OR: avhrr_ndvi_comb startup.txt " << endl;
		cout << "************ sample startup.txt *************" << endl;
		cout << "#indir" << endl;
		cout << "/avhrr_data/" << endl;
		cout << "#outdir" << endl;
		cout << "/avhrr_data_out/" << endl;
		cout << "#method" << endl;
		cout << "max/aver" << endl;
		cout << "#type" << endl;
		cout << "fiv/ten/mon/sea/yea" << endl;
		cout << "#comb" << endl;
		cout << "datacombinationfornoaandvi" << endl;

		cout << "**********************************************" << endl;


		exit(101);
	}
	
	std::string inDir, outDir, method , inType, combination ;
	if (argc == 2)
	{
		string startup = argv[1];
		inDir = wft_getValueFromExtraParamsFile(startup, "#indir", true);
		outDir = wft_getValueFromExtraParamsFile(startup, "#outdir", true);
		method = wft_getValueFromExtraParamsFile(startup, "#method", true);
		inType = wft_getValueFromExtraParamsFile(startup, "#type", true);
		combination = wft_getValueFromExtraParamsFile(startup, "#comb", true);
	}
	else {
		wft_has_param(argc, argv, "-indir", inDir, true);
		wft_has_param(argc, argv, "-outdir", outDir, true);
		wft_has_param(argc, argv, "-method", method, true);
		wft_has_param(argc, argv, "-type", inType, true);
		wft_has_param(argc, argv, "-comb", combination, true);
	}


	std::vector<std::string> allFiles;
	wft_get_allfiles(inDir , allFiles );
	if (allFiles.size() == 0)
	{
		std::cout << "No files under "<<inDir<<" . out." << std::endl;
		exit(102);
	}

	int minYear = 9999;
	int maxYear = -9999;
	for (size_t i = 0; i < allFiles.size(); ++i)
	{
		WYmd ymd1 = dateOfAvhrrFilename(allFiles[i]);
		if (ymd1.year > 0)
		{
			if (ymd1.year > maxYear) maxYear = ymd1.year;
			if (ymd1.year < minYear) minYear = ymd1.year;
		}
	}
	if (maxYear < 0) {
		std::cout << "maxYear invalid " << maxYear << " . out." << std::endl;
		exit(103);
	}


	printCurrentTime();
	std::cout << "processing... " << std::endl;

	if (wft_has_tag(argc,argv , "-monthonly") || inType=="" || inType=="mon" )
	{
		processAllMonth(allFiles, outDir, method, minYear, maxYear , combination);
	}
	
	if (inType == "ten" || inType == "")
	{
		processAllXun(allFiles, outDir, method, minYear, maxYear , combination );
	}
	
	if (inType == "fiv" || inType == "")
	{
		processAllHou(allFiles, outDir, method, minYear, maxYear , combination );
	}
	
	if (inType == "sea" || inType == "")
	{
		processAllSeason(allFiles, outDir, method, minYear, maxYear , combination );
	}
	
	if( inType == "" || inType=="yea" ) {		
		processAllYear(allFiles, outDir, method, minYear, maxYear , combination);
	}

	


	printCurrentTime();
	std::cout << "done." << std::endl;

	return 0;
}



//����ÿ��Ѯ�����ݣ����кϲ�
void processXunData(std::vector<std::string> xunFileVector, std::string outDir, std::string cbMethod, string combination )
{
	if (xunFileVector.size() == 0) return;

	if (*outDir.rbegin() != '/')
	{
		outDir += "/";
	}

	std::string fname0 = xunFileVector[0];
	std::string fname1 = *xunFileVector.rbegin();

	WYmd ymd1 = dateOfAvhrrFilename(fname1);
	bool xunIsOk = false;
	xunIsOk = true;//2017-10-9�����м����㼸�졣


	if (xunIsOk)
	{//Ѯ�����������ļ���

		std::string outFilename = outDir + makeAvhrrCombinationFilename(fname0, fname1, "_ten.tif" , cbMethod);
		if (wft_test_file_exists(outFilename) == false)
		{//����ļ������ڣ���������
			std::string temptxt = outFilename + ".txt";
			std::ofstream txtfs(temptxt.c_str());
			for (size_t iday = 0; iday < xunFileVector.size(); ++iday)
			{
				txtfs << xunFileVector[iday] << std::endl;
			}
			txtfs.close();

			//v0.3a
			std::string cmd = combination + " "; 
			cmd += std::string(" -method ") + cbMethod + " -vmin -1000 -vmax 10000 ";
			cmd += std::string(" -out ") + outFilename;
			cmd += std::string(" -infilestxt ") + temptxt;


			int ret = 0;
			ret = std::system(cmd.c_str());
			std::cout << "datacombination call return code:" << ret << std::endl;
		}
		else {
			std::cout << outFilename << " file exist , no need to process." << std::endl;
		}
	}
	else {
		std::cout << " data not enough for xun, no need to process." << std::endl;
	}
}



//����ÿ��������ݣ����кϲ�
void processHouData(std::vector<std::string> houFileVector, std::string outDir, std::string cbMethod, string combination )
{
	if (houFileVector.size() == 0) return;

	if (*outDir.rbegin() != '/')
	{
		outDir += "/";
	}

	std::string fname0 = houFileVector[0];
	std::string fname1 = *houFileVector.rbegin();

	WYmd ymd1 = dateOfAvhrrFilename(fname1);
	bool ok  = true ;
	//2017-10-9�����м����㼸�졣


	if ( ok )
	{//

		std::string outFilename = outDir + makeAvhrrCombinationFilename(fname0, fname1, "_fiv.tif" , cbMethod );
		if (wft_test_file_exists(outFilename) == false)
		{//����ļ������ڣ���������
			std::string temptxt = outFilename + ".txt";
			std::ofstream txtfs(temptxt.c_str());
			for (size_t iday = 0; iday < houFileVector.size(); ++iday)
			{
				txtfs << houFileVector[iday] << std::endl;
			}
			txtfs.close();

			//v0.3a
			std::string cmd = combination + " " ;
			cmd += std::string(" -method ") + cbMethod + " -vmin -1000 -vmax 10000 ";
			cmd += std::string(" -out ") + outFilename;
			cmd += std::string(" -infilestxt ") + temptxt;


			int ret = 0;
			ret = std::system(cmd.c_str());
			std::cout << "datacombination call return code:" << ret << std::endl;
		}
		else {
			std::cout << outFilename << " file exist , no need to process." << std::endl;
		}
	}
	else {
		std::cout << " data not enough for xun, no need to process." << std::endl;
	}
}




//����ÿ���µ����ݣ����кϲ�
void processMonthData(std::vector<std::string> fileVector, std::string outDir, std::string cbMethod, string combination )
{
	if (fileVector.size() == 0) return;

	if (*outDir.rbegin() != '/')
	{
		outDir += "/";
	}

	std::string fname0 = fileVector[0];
	std::string fname1 = *fileVector.rbegin();

	WYmd ymd1 = dateOfAvhrrFilename(fname1);
	bool isDataEnough = false;

	isDataEnough = true;//2017-10-9�����м����㼸�졣
	if (isDataEnough)
	{//�������������ļ���

		std::string outFilename = outDir + makeAvhrrCombinationFilename(fname0, fname1, "_mon.tif" , cbMethod );
		if (wft_test_file_exists(outFilename) == false)
		{//����ļ������ڣ���������
			std::string temptxt = outFilename + ".txt";
			std::ofstream txtfs(temptxt.c_str());
			for (size_t iday = 0; iday < fileVector.size(); ++iday)
			{
				txtfs << fileVector[iday] << std::endl;
			}
			txtfs.close();

			std::string cmd = combination + " ";
			cmd += std::string(" -method ") + cbMethod + " -vmin -1000 -vmax 10000 ";
			cmd += std::string(" -out ") + outFilename;
			cmd += std::string(" -infilestxt ") + temptxt;

			int ret = 0;
			ret = std::system(cmd.c_str());
			std::cout << "datacombination call return code:" << ret << std::endl;
		}
		else {
			std::cout << outFilename << " file exist , no need to process." << std::endl;
		}
	}
	else {
		std::cout << " data not enough for month, no need to process." << std::endl;
	}
}

//����ÿ���������ݣ����кϲ�
void processSeasonData(std::vector<std::string> fileVector, std::string outDir, std::string cbMethod, string combination )
{
	if (fileVector.size() == 0) return;

	if (*outDir.rbegin() != '/')
	{
		outDir += "/";
	}

	std::string fname0 = fileVector[0];
	std::string fname1 = *fileVector.rbegin();

	WYmd ymd1 = dateOfAvhrrFilename(fname1);
	bool isDataEnough = false;
	
	isDataEnough = true;//2017-10-9�����м����㼸�졣
	if (isDataEnough)
	{//�������������ļ���

		std::string outFilename = outDir + makeAvhrrCombinationFilename(fname0, fname1, "_sea.tif" , cbMethod );
		if (wft_test_file_exists(outFilename) == false)
		{//����ļ������ڣ���������
			std::string temptxt = outFilename + ".txt";
			std::ofstream txtfs(temptxt.c_str());
			for (size_t iday = 0; iday < fileVector.size(); ++iday)
			{
				txtfs << fileVector[iday] << std::endl;
			}
			txtfs.close();

			std::string cmd = combination + " ";
			cmd += std::string(" -method ") + cbMethod + " -vmin -1000 -vmax 10000 ";
			cmd += std::string(" -out ") + outFilename;
			cmd += std::string(" -infilestxt ") + temptxt;

			int ret = 0;
			ret = std::system(cmd.c_str());
			std::cout << "datacombination call return code:" << ret << std::endl;
		}
		else {
			std::cout << outFilename << " file exist , no need to process." << std::endl;
		}
	}
	else {
		std::cout << " data not enough for season, no need to process." << std::endl;
	}
}


//����ÿ��������ݣ����кϲ�
void processYearData(std::vector<std::string> fileVector, std::string outDir, std::string cbMethod , string combination)
{
	if (fileVector.size() == 0) return;

	if (*outDir.rbegin() != '/')
	{
		outDir += "/";
	}

	std::string fname0 = fileVector[0];
	std::string fname1 = *fileVector.rbegin();

	WYmd ymd1 = dateOfAvhrrFilename(fname1);
	bool isDataEnough = false;
	
	isDataEnough = true;//2017-10-9�����м����㼸�졣
	if (isDataEnough)
	{//�������������ļ���
		std::string outFilename = outDir + makeAvhrrCombinationFilename(fname0, fname1, "_yea.tif" , cbMethod );
		if (wft_test_file_exists(outFilename) == false)
		{//����ļ������ڣ���������
			std::string temptxt = outFilename + ".txt";
			std::ofstream txtfs(temptxt.c_str());
			for (size_t iday = 0; iday < fileVector.size(); ++iday)
			{
				txtfs << fileVector[iday] << std::endl;
			}
			txtfs.close();

			std::string cmd = combination + " ";
			cmd += std::string(" -method ") + cbMethod + " -vmin -1000 -vmax 10000 ";
			cmd += std::string(" -out ") + outFilename;
			cmd += std::string(" -infilestxt ") + temptxt;

			int ret = 0;
			ret = std::system(cmd.c_str());
			std::cout << "datacombination call return code:" << ret << std::endl;
		}
		else {
			std::cout << outFilename << " file exist , no need to process." << std::endl;
		}
	}
	else {
		std::cout << " data not enough for year, no need to process." << std::endl;
	}
}



void processAllXun(std::vector<std::string>& filesVector ,std::string outDir , std::string cbMethod , int startYear , int endYear, string combination )
{//ÿһ��ݵ��������ݶ���һ���ļ�����
	//��������Ѯ���ݣ���0101��ʼһֱ���㵽1231��ÿ10��һ�������
	//�����һ����������Ľ����outdir���ҵ�����ô������һ���������䣬ֱ��12��31��ֹ��
	int todayYmd = wft_current_dateymd_int();
	for (int year = startYear; year <= endYear; ++year)
	{
		for (int imon = 1; imon < 13; ++imon)
		{
			std::vector<std::string> xunVectorArray[3];
			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d", year , imon, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					int xunIndex = 0;
					if (iday < 11) xunIndex = 0;
					else if (iday < 21) xunIndex = 1;
					else xunIndex = 2;
					xunVectorArray[xunIndex].push_back(filepath); 
				}
			}//end of day
			for (int ixun = 0; ixun < 3; ++ixun)
			{
				if (ixun == 0)
				{
					int ymd1 = year * 10000 + imon * 100 + 11;
					if (todayYmd < ymd1) continue;
				}
				else if (ixun == 1)
				{
					int ymd1 = year * 10000 + imon * 100 + 21;
					if (todayYmd < ymd1) continue;
				}
				else  
				{
					int ymd1 = year * 10000 + imon * 100 + 32;
					if (todayYmd < ymd1) continue;
				}
				 processXunData(xunVectorArray[ixun], outDir, cbMethod, combination);
			}
		}
	}
	
}




void processAllHou(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination )
{//ÿһ��ݵ��������ݶ���һ���ļ�����
 //������������ݣ����죩����0101��ʼһֱ���㵽1231��ÿ5��һ�������
 //�����һ����������Ľ����outdir���ҵ�����ô������һ���������䣬ֱ��12��31��ֹ��
	int todayYmd = wft_current_dateymd_int();
	for (int year = startYear; year <= endYear; ++year)
	{
		for (int imon = 1; imon < 13; ++imon)
		{
			std::vector<std::string> houVectorArray[6];
			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d", year, imon, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					int houIndex = 0;
					if (iday < 6 ) houIndex = 0;
					else if (iday < 11 ) houIndex = 1;
					else if (iday < 16) houIndex = 2;
					else if (iday < 21) houIndex = 3;
					else if (iday < 26) houIndex = 4;
					else houIndex = 5;
					houVectorArray[houIndex].push_back(filepath);
				}
			}//end of day
			for (int ihou = 0; ihou < 6; ++ihou)
			{
				if (ihou == 0)
				{
					int ymd1 = year * 10000 + imon * 100 + 6;
					if (todayYmd < ymd1) continue;
				}
				else if (ihou == 1)
				{
					int ymd1 = year * 10000 + imon * 100 + 11;
					if (todayYmd < ymd1) continue;
				}
				else if (ihou == 2)
				{
					int ymd1 = year * 10000 + imon * 100 + 16;
					if (todayYmd < ymd1) continue;
				}
				else if (ihou == 3)
				{
					int ymd1 = year * 10000 + imon * 100 + 21;
					if (todayYmd < ymd1) continue;
				}
				else if (ihou == 4)
				{
					int ymd1 = year * 10000 + imon * 100 + 26;
					if (todayYmd < ymd1) continue;
				}
				else {
					int ymd1 = year * 10000 + imon * 100 + 32 ;
					if (todayYmd < ymd1) continue;
				}

				processHouData(houVectorArray[ihou], outDir, cbMethod,combination);
			}
		}
	}

}



void processAllMonth(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod , int startYear , int endYear , string combination )
{//ÿһ��ݵ��������ݶ���һ���ļ�����
 //�������������ݣ���0101��ʼһֱ���㵽1231��ÿ��һ�������
 //�����һ����������Ľ����outdir���ҵ�����ô������һ���������䣬ֱ��12��31��ֹ��

	int todayYmd = wft_current_dateymd_int();

	for (int year = startYear; year <= endYear; ++year)
	{
		for (int imon = 1; imon < 13; ++imon)
		{
			std::vector<std::string> monthVector;
			int ymd0 = year * 10000 + imon * 100 + 0;
			int ymd1 = year * 10000 + imon * 100 + 32 ;
			if (todayYmd < ymd1 ) continue;

			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d",year, imon, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					monthVector.push_back(filepath); 
				}
			}//end of day
			processMonthData(monthVector, outDir, cbMethod, combination );
		}
	}
}


void processAllYear(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination )
{//ÿһ��ݵ��������ݶ���һ���ļ�����
 //�������������ݣ���0101��ʼһֱ���㵽1231��ÿ��һ�������
 //

	int todayYmd = wft_current_dateymd_int();

	for (int year = startYear; year <= endYear; ++year)
	{
		int ymd0 = year * 10000 + 1 * 100 + 0;
		int ymd1 = year * 10000 + 12 * 100 + 32;
		if ( todayYmd < ymd1 ) continue;

		std::vector<std::string> yearFiles;
		for (int imon = 1; imon < 13; ++imon)
		{
			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d", year, imon, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					yearFiles.push_back(filepath);
				}
			}//end of day
		}
		processYearData(yearFiles, outDir, cbMethod , combination );
	}
}



void processAllSeason(std::vector<std::string>& filesVector, std::string outDir, std::string cbMethod, int startYear, int endYear, string combination )
{//ÿһ��ݵ��������ݶ���һ���ļ�����
 //�������㼾���ݣ���0101��ʼһֱ���㵽1231��ÿ��һ�������
 //
	int todayYmd = wft_current_dateymd_int();
	for (int year = startYear; year <= endYear; ++year)
	{
		std::vector<std::string> seasonFilesArray[4] ;
		int lastYear = year - 1;
		if (lastYear >= startYear)
		{//last year
			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d", lastYear, 12, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					seasonFilesArray[0].push_back(filepath);
				}
			}
		}
		for (int imon = 1; imon < 12; ++imon)
		{
			for (int iday = 1; iday < 32; ++iday)
			{
				char tdate[10];
				sprintf(tdate, "%04d%02d%02d", year, imon, iday);
				std::string tdateStr(tdate);
				std::cout << tdateStr << "\t";
				std::string filepath = lookingForAvhrrFilenameByDate8(filesVector, tdateStr);
				if (filepath.length() > 1) {
					int iseason = imon / 3;
					seasonFilesArray[iseason].push_back(filepath);
				}
			}//end of day
		}

		int ymd0 = lastYear * 10000 + 12 * 100 + 0;
		int ymd1 = year * 10000 + 2 * 100 + 32;
		if (todayYmd > ymd1)
		{
			processSeasonData(seasonFilesArray[0], outDir, cbMethod, combination);
		}

		int ymd2 = year * 10000 + 5 * 100 + 32;
		if (todayYmd > ymd2)
		{
			processSeasonData(seasonFilesArray[1], outDir, cbMethod, combination);
		}
		
		int ymd3 = year * 10000 + 8 * 100 + 32;
		if (todayYmd > ymd3)
		{
			processSeasonData(seasonFilesArray[2], outDir, cbMethod, combination);
		}
		
		int ymd4 = year * 10000 + 11 * 100 + 32;
		if (todayYmd > ymd4)
		{
			processSeasonData(seasonFilesArray[3], outDir, cbMethod, combination);
		}
	}
}


