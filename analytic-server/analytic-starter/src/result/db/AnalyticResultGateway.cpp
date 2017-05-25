/*
 * AnalyticResultGateway.cpp
 *
 *  Created on: May 12, 2017
 *      Author: anjana
 */

#include "AnalyticResultGateway.hpp"

namespace result {
namespace db {

const std::string AnalyticResultGateway::_SELECT_ANALYTIC_RESULTS_FOR_RAPP_INST_SQL = "SELECT ar.id as result_id, ar.analytic_instance_id, ar.timestamp, ar.result "
		"FROM analytic_results as ar, results_app_registrations as rar WHERE ar.analytic_instance_id = rar.analytic_instance_id AND "
		"rar.results_app_instance_id = ? AND (ar.id, rar.results_app_instance_id)  NOT IN (SELECT sar.analytic_result_id as result_id, "
		"sar.results_app_instance_id as inst_id  FROM sent_analytic_results as sar) ORDER BY ar.id";

const std::string AnalyticResultGateway::_SELECT_RESULT_IMAGES_SQL = "SELECT image_file_path FROM images WHERE analytic_result_id = ?";

const std::string AnalyticResultGateway::_SELECT_RESULT_VIDEOS_SQL = "SELECT video_file_path FROM videos WHERE analytic_result_id = ?";

const std::string AnalyticResultGateway::_INSERT_SENT_RESULT_SQL = "INSERT INTO sent_analytic_results (analytic_result_id, results_app_instance_id) VALUES(?,?)";

AnalyticResultGateway::AnalyticResultGateway()
{
	try
	{
		_pDbConnectorPtr = new DbConnector();
		_pConnectionPtr = (*_pDbConnectorPtr).getConnection();
	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "Error while initializing the AnalyticResultGateway. ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
	}
	catch(opencctv::Exception& e)
	{
		throw opencctv::Exception(e);
	}
}

void AnalyticResultGateway::findAnalyticResultsForRAppInst(const unsigned int iRAppInstId,
		std::vector<result::db::dto::AnalyticResult>& vAnalyticResults)

{
	try
	{
		sql::PreparedStatement* pStatementPtr;
		pStatementPtr = (*_pConnectionPtr).prepareStatement(_SELECT_ANALYTIC_RESULTS_FOR_RAPP_INST_SQL);
		(*pStatementPtr).setInt(1, iRAppInstId);
		sql::ResultSet* pResultsPtr = (*pStatementPtr).executeQuery();
		result::db::dto::AnalyticResult analyticResult;
		while((*pResultsPtr).next())
		{
			analyticResult.setResultsId((*pResultsPtr).getUInt("result_id"));
			analyticResult.setAnalyticInstId((*pResultsPtr).getUInt("analytic_instance_id"));
			analyticResult.setTimestamp((*pResultsPtr).getString("timestamp"));
			analyticResult.setResult((*pResultsPtr).getString("result"));
			vAnalyticResults.push_back(analyticResult);
		}
		(*pResultsPtr).close();
		(*pStatementPtr).close();
		delete pResultsPtr; pResultsPtr = NULL;
		delete pStatementPtr; pStatementPtr = NULL;

	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "AnalyticResultGateway::findAnalyticResultsForRAppInst: ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
		// TODO: log
	}
}

void AnalyticResultGateway::findResultImages(const unsigned int iResultId,
		std::map<std::string, std::string>& mapImages)
{
	try
	{
		sql::PreparedStatement* pStatementPtr;
		pStatementPtr = (*_pConnectionPtr).prepareStatement(_SELECT_RESULT_IMAGES_SQL);
		(*pStatementPtr).setInt(1, iResultId);
		sql::ResultSet* pResultsPtr = (*pStatementPtr).executeQuery();
		std::string sImagePath;
		while((*pResultsPtr).next())
		{
			sImagePath = (*pResultsPtr).getString("image_file_path");
			boost::filesystem::path sPath(sImagePath);
			mapImages.insert(std::pair<std::string,std::string>(sPath.filename().string(),sImagePath));
		}
		(*pResultsPtr).close();
		(*pStatementPtr).close();
		delete pResultsPtr; pResultsPtr = NULL;
		delete pStatementPtr; pStatementPtr = NULL;

	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "AnalyticResultGateway::findResultImages : ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
		// TODO: log
	}
}

void AnalyticResultGateway::findResultVideos(const unsigned int iResultId,
		std::map<std::string, std::string>& mapVideos)
{
	try
	{
		sql::PreparedStatement* pStatementPtr;
		pStatementPtr = (*_pConnectionPtr).prepareStatement(_SELECT_RESULT_VIDEOS_SQL);
		(*pStatementPtr).setInt(1, iResultId);
		sql::ResultSet* pResultsPtr = (*pStatementPtr).executeQuery();
		std::string sVideoPath;
		while((*pResultsPtr).next())
		{
			sVideoPath = (*pResultsPtr).getString("video_file_path");
			boost::filesystem::path sPath(sVideoPath);
			mapVideos.insert(std::pair<std::string,std::string>(sPath.filename().string(),sVideoPath));
		}
		(*pResultsPtr).close();
		(*pStatementPtr).close();
		delete pResultsPtr; pResultsPtr = NULL;
		delete pStatementPtr; pStatementPtr = NULL;

	}catch(sql::SQLException &e)
	{
		std::string sErrorMsg = "AnalyticResultGateway::findResultImages : ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
		// TODO: log
	}
}

int AnalyticResultGateway::insertToSentResults(const unsigned int iResultId,
		const unsigned int iRAppInstId)
{
	int result = 0;

	try{
		sql::PreparedStatement* pStatementPtr;
		pStatementPtr = (*_pConnectionPtr).prepareStatement(_INSERT_SENT_RESULT_SQL);
		(*pStatementPtr).setInt(1, iResultId);
		(*pStatementPtr).setInt(1, iRAppInstId);
		result = (*pStatementPtr).executeUpdate();

	}catch(sql::SQLException &e)
	{
		result = -1;
		std::string sErrorMsg = "AnalyticResultGateway::insertToSentResults : ";
		throw opencctv::Exception(sErrorMsg.append(e.what()));
		// TODO: log
	}
	return result;
}

AnalyticResultGateway::~AnalyticResultGateway()
{
	delete _pConnectionPtr; _pConnectionPtr = NULL;
	delete _pDbConnectorPtr; _pDbConnectorPtr = NULL;
}

} /* namespace db */
} /* namespace result */