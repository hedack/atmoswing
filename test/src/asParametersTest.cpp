#include "include_tests.h"
#include "asParameters.h"

#include "UnitTest++.h"

namespace
{

TEST(LoadFromFile)
{
    wxString filepath = wxFileName::GetCwd();
    filepath.Append("/files/asParametersTestFile01.xml");

    asParameters params;
    params.LoadFromFile(filepath);

    CHECK_EQUAL(1962, params.GetArchiveYearStart());
    CHECK_EQUAL(2008, params.GetArchiveYearEnd());
    CHECK_EQUAL(24, params.GetTimeArrayAnalogsTimeStepHours());
    CHECK_EQUAL(24, params.GetTimeArrayTargetTimeStepHours());
    CHECK_EQUAL(60, params.GetTimeArrayAnalogsIntervalDays());
    CHECK_EQUAL(60, params.GetTimeArrayAnalogsExcludeDays());
    CHECK_EQUAL(true, params.GetTimeArrayAnalogsMode().IsSameAs("DaysInterval"));
    CHECK_EQUAL(true, params.GetTimeArrayTargetMode().IsSameAs("Simple"));

    CHECK_EQUAL(true, params.GetMethodName(0).IsSameAs("Analogs"));
    CHECK_EQUAL(100, params.GetAnalogsNumber(0));

    CHECK_EQUAL(false, params.NeedsPreprocessing(0,0));
    CHECK_EQUAL(true, params.GetPredictorDatasetId(0,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPredictorDataId(0,0).IsSameAs("hgt"));
    CHECK_EQUAL(500, params.GetPredictorLevel(0,0));
    CHECK_EQUAL(24, params.GetPredictorDTimeHours(0,0));
    CHECK_EQUAL(1, params.GetPredictorDTimeDays(0,0));
    CHECK_EQUAL(0, params.GetPredictorTimeHour(0,0));
    CHECK_EQUAL(true, params.GetPredictorGridType(0,0).IsSameAs("Regular"));
    CHECK_EQUAL(-10, params.GetPredictorUmin(0,0));
    CHECK_EQUAL(9, params.GetPredictorUptsnb(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,0));
    CHECK_EQUAL(30, params.GetPredictorVmin(0,0));
    CHECK_EQUAL(5, params.GetPredictorVptsnb(0,0));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,0));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,0).IsSameAs("S1"));
    CHECK_CLOSE(0.6, params.GetPredictorWeight(0,0), 0.0001);

    CHECK_EQUAL(true, params.NeedsPreprocessing(0,1));
    CHECK_EQUAL(true, params.GetPreprocessMethod(0,1).IsSameAs("Gradients"));
    CHECK_EQUAL(true, params.GetPreprocessDatasetId(0,1,0).IsSameAs("NCEP_R-1"));
    CHECK_EQUAL(true, params.GetPreprocessDataId(0,1,0).IsSameAs("hgt"));
    CHECK_EQUAL(1000, params.GetPreprocessLevel(0,1,0));
    CHECK_EQUAL(12, params.GetPreprocessDTimeHours(0,1,0));
    CHECK_EQUAL(1000, params.GetPredictorLevel(0,1));
    CHECK_EQUAL(12, params.GetPredictorDTimeHours(0,1));
    CHECK_EQUAL(0.5, params.GetPredictorDTimeDays(0,1));
    CHECK_EQUAL(12, params.GetPredictorTimeHour(0,1));
    CHECK_EQUAL(-15, params.GetPredictorUmin(0,1));
    CHECK_EQUAL(11, params.GetPredictorUptsnb(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorUstep(0,1));
    CHECK_EQUAL(35, params.GetPredictorVmin(0,1));
    CHECK_EQUAL(7, params.GetPredictorVptsnb(0,1));
    CHECK_EQUAL(2.5, params.GetPredictorVstep(0,1));
    CHECK_EQUAL(true, params.GetPredictorCriteria(0,1).IsSameAs("S1"));
    CHECK_CLOSE(0.4, params.GetPredictorWeight(0,1), 0.0001);

    CHECK_EQUAL(40, params.GetPredictandStationId());
}

}