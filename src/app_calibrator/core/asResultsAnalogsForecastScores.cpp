#include "asResultsAnalogsForecastScores.h"

#include "asFileNetcdf.h"
#include "asParametersScoring.h"


asResultsAnalogsForecastScores::asResultsAnalogsForecastScores()
:
asResults()
{
    ThreadsManager().CritSectionConfig().Enter();
    wxFileConfig::Get()->Read("/Calibration/IntermediateResults/SaveForecastScores", &m_SaveIntermediateResults, false);
    wxFileConfig::Get()->Read("/Calibration/IntermediateResults/LoadForecastScores", &m_LoadIntermediateResults, false);
    ThreadsManager().CritSectionConfig().Leave();
}

asResultsAnalogsForecastScores::~asResultsAnalogsForecastScores()
{
    //dtor
}

void asResultsAnalogsForecastScores::Init(asParametersScoring &params)
{
    if(m_SaveIntermediateResults || m_LoadIntermediateResults) BuildFileName(params);

    // Resize to 0 to avoid keeping old results
    m_TargetDates.resize(0);
    m_ForecastScores.resize(0);
}

void asResultsAnalogsForecastScores::BuildFileName(asParametersScoring &params)
{
    ThreadsManager().CritSectionConfig().Enter();
    m_FilePath = wxFileConfig::Get()->Read("/StandardPaths/IntermediateResultsDir", asConfig::GetDefaultUserWorkingDir() + "IntermediateResults" + DS);
    ThreadsManager().CritSectionConfig().Leave();
    m_FilePath.Append(DS);
    m_FilePath.Append(wxString::Format("AnalogsForecastScores_id%d_step%d", params.GetPredictandStationId(), m_CurrentStep));
    m_FilePath.Append(".nc");
}

bool asResultsAnalogsForecastScores::Save(const wxString &AlternateFilePath)
{
    // If we don't want to save, skip
    if(!m_SaveIntermediateResults) return false;
    wxString message = _("Saving intermediate file: ") + m_FilePath;
    asLogMessage(message);

    // Get the file path
    wxString ResultsFile;
    if (AlternateFilePath.IsEmpty())
    {
        ResultsFile = m_FilePath;
    }
    else
    {
        ResultsFile = AlternateFilePath;
    }

    // Get the elements size
    size_t Ntime = m_ForecastScores.rows();

    ThreadsManager().CritSectionNetCDF().Enter();

    // Create netCDF dataset: enter define mode
    asFileNetcdf ncFile(ResultsFile, asFileNetcdf::Replace);
    if(!ncFile.Open())
    {
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }

    // Define dimensions. Time is the unlimited dimension.
    ncFile.DefDim("time");

    // The dimensions name array is used to pass the dimensions to the variable.
    VectorStdString DimNames1;
    DimNames1.push_back("time");

    // Define variables: the scores and the corresponding dates
    ncFile.DefVar("targetdates", NC_FLOAT, 1, DimNames1);
    ncFile.DefVar("forecastscores", NC_FLOAT, 1, DimNames1);

    // Put attributes
    DefTargetDatesAttributes(ncFile);
    DefForecastScoresAttributes(ncFile);

    // End definitions: leave define mode
    ncFile.EndDef();

    // Provide sizes for variables
    size_t start1D[] = {0};
    size_t count1D[] = {Ntime};

    // Write data
    ncFile.PutVarArray("targetdates", start1D, count1D, &m_TargetDates(0));
    ncFile.PutVarArray("forecastscores", start1D, count1D, &m_ForecastScores(0));

    // Close:save new netCDF dataset
    ncFile.Close();

    ThreadsManager().CritSectionNetCDF().Leave();

    return true;
}

bool asResultsAnalogsForecastScores::Load(const wxString &AlternateFilePath)
{
    // If we don't want to save or the file doesn't exist
    if(!m_LoadIntermediateResults) return false;
    if(!Exists()) return false;
    if(m_CurrentStep!=0) return false;

    // Get the file path
    wxString ResultsFile;
    if (AlternateFilePath.IsEmpty())
    {
        ResultsFile = m_FilePath;
    }
    else
    {
        ResultsFile = AlternateFilePath;
    }

    ThreadsManager().CritSectionNetCDF().Enter();

    // Open the NetCDF file
    asFileNetcdf ncFile(ResultsFile, asFileNetcdf::ReadOnly);
    if(!ncFile.Open())
    {
        ThreadsManager().CritSectionNetCDF().Leave();
        return false;
    }

    // Get the elements size
    int TimeLength = ncFile.GetDimLength("time");

    // Get time and data
    m_TargetDates.resize( TimeLength );
    ncFile.GetVar("targetdates", &m_TargetDates[0]);
    m_ForecastScores.resize( TimeLength );
    ncFile.GetVar("forecastscores", &m_ForecastScores[0]);

    ncFile.Close();

    ThreadsManager().CritSectionNetCDF().Leave();

    return true;
}