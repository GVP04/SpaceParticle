
// SpaceOM.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "SpaceOM.h"
#include "SpaceOMDlg.h"
#include <direct.h>
#include "CommonFunc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HPEN PenGrSpeed;
HPEN PenGrAcc;
HPEN PenDGrSpeed;
HPEN PenDGrAcc;
HPEN PenGrTrace;
HPEN PenDGrTrace;


// CSpaceOMApp

BEGIN_MESSAGE_MAP(CSpaceOMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSpaceOMApp construction
/*
UINT DoCalc_111(LPVOID pParam)
{
	int cc;
	while (1)
	{
		int ii = GetTickCount();
		if (ii != 0)
		{
			int j = (ii + 1234556) / ii;
			cc = j;
		}
	}
}
*/

void my_Test();


CSpaceOMApp::CSpaceOMApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance


    _mkdir("D:\\SParticle");
	_chdir("D:\\SParticle");
    _mkdir("BMP");
    _mkdir("BMP\\VIDEO_SRC");

    //SPos Point_A = {0}, Point_B = { 0 }, Point_C = { 0 };

    //Point_A.X = 0.5;
    //Point_B.X = -0.5;

    //Point_C.X = 0.1;
    //Point_C.Y = -1.0;

    //SPos Point_Middle_AB;



    //Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);
    //SPos PlaneNormal;
    //PlaneNormal.SET_AS_DELTA(Point_A, Point_Middle_AB);
    //SPos pojPoint;
    //pojPoint.SET_AS_DELTA(Point_C, Point_Middle_AB);
    //pojPoint.SET_AS_PROJECTION_ONTO_PLANE(PlaneNormal);
    //pojPoint.SET_LENGHT(sqrt(1.0 - PlaneNormal.LENPWR2()));
    //pojPoint += Point_Middle_AB;
    //double dist011 = Point_A.GetDistance(pojPoint);
    //double dist111 = Point_B.GetDistance(pojPoint);


    //ASSERT(0);
    //ASSERT(0);
    //ASSERT(0);



    //SPos Point_A = {0}, Point_B = { 0 }, Point_To_Find = { 0 };
    //Point_A.X = -3;
    //Point_B.X = 3;
    //Point_To_Find.X = 1;
    //Point_To_Find.Y = 5;
    //double EequiDistant = 5;


    //SPos Point_Middle_AB;
    //Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);

    //SPos Delta_BA; Delta_BA.SET_AS_DELTA(Point_B, Point_A);
    //double L2 = Delta_BA.LENPWR2();

    //if (L2 > 1e-14)
    //{
    //    SPos PTF_To_MAB = Point_Middle_AB;
    //    PTF_To_MAB -= Point_To_Find;
    //    Delta_BA *= PTF_To_MAB.DOT(Delta_BA) / L2;
    //    Point_To_Find += Delta_BA;
    //    Point_To_Find -= Point_Middle_AB;
    //    Point_To_Find.SET_LENGHT_by_Pwr2(EequiDistant * EequiDistant - L2 * 0.25);
    //    Point_To_Find += Point_Middle_AB;
    //}
    //else
    //{
    //    Point_To_Find -= Point_Middle_AB;
    //    Point_To_Find.SET_LENGHT(EequiDistant);
    //    Point_To_Find += Point_Middle_AB;
    //}




    //SPos Delta_BA; Delta_BA.SET_AS_DELTA(Point_B, Point_A);
    //double L2 = Delta_BA.LENPWR2();

    //if (L2 > 1e-14)
    //{
    //    SPos Point_Middle_AB;  Point_Middle_AB.SET_AS_MIDDLE_POINT(Point_A, Point_B);
    //    Point_Middle_AB -= Point_To_Find;
    //    Delta_BA *= Point_Middle_AB.DOT(Delta_BA) / L2;
    //    Point_To_Find += Delta_BA;
    //}





    ////my_Test();

}


// The one and only CSpaceOMApp object

CSpaceOMApp theApp;


// CSpaceOMApp initialization

BOOL CSpaceOMApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	PenGrSpeed = CreatePen(PS_DOT, 1, 0xB0D0B0);
	PenGrAcc = CreatePen(PS_DOT, 1, 0xD0B0B0);
	PenDGrSpeed = CreatePen(PS_DOT, 1, 0x308030);
	PenDGrAcc = CreatePen(PS_DOT, 1, 0x803030);
	PenGrTrace = CreatePen(PS_SOLID, 1, 0xB0B0B0);
	PenDGrTrace = CreatePen(PS_SOLID, 1, 0x404040);

	dlg = new CSpaceOMDlg;
	m_pMainWnd = dlg;
	INT_PTR nResponse = dlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	DeleteObject(PenDGrAcc);
	DeleteObject(PenDGrSpeed);
	DeleteObject(PenGrAcc);
	DeleteObject(PenGrSpeed);
	DeleteObject(PenGrTrace);
	DeleteObject(PenDGrTrace);

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.

	delete dlg;
	return FALSE;
}















/*


class PointDynamics {
publicpublic:
    // Расчёт третьей производной (рывка)
    static Vector3D calculateJerk(const Vector3D& position, const Vector3D& velocity) {
        double S = position.magnitude();
        if (S <= 1.0) {
            std::cerr << "Ошибка: S должно быть > 1\n";
            return Vector3D();
        }

        double S_sq = S * S;
        double a_magnitude = 1.0 / (S_sq - 1.0);
        double d_a_dt = -2.0 * S * position.dot(velocity) / ((S_sq - 1.0) * (S_sq - 1.0));

        // Единичный вектор направления
        Vector3D n = position * (1.0 / S);

        // Производная единичного вектора
        Vector3D dn_dt = (velocity * (1.0 / S)) -
            (position * (position.dot(velocity) / (S * S_sq)));

        // Рывок
        Vector3D jerk = n * d_a_dt + (dn_dt * a_magnitude);
        return jerk;
    }

    // Расчёт четвёртой производной (snap) численно
    static Vector3D calculateSnap(const Vector3D& position,
        const Vector3D& velocity,
        double dt = 1e-6) {
        // Расчёт рывка в текущей точке
        Vector3D currentJerk = calculateJerk(position, velocity);

        // Приращение для численного дифференцирования
        Vector3D newPosition = position + velocity * dt;
        Vector3D newVelocity = velocity + calculateAcceleration(position) * dt;

        // Расчёт рывка в смещённой точке
        Vector3D nextJerk = calculateJerk(newPosition, newVelocity);

        // Численная производная рывка
        Vector3D snap = (nextJerk - currentJerk) * (1.0 / dt);
        return snap;
    }

private:
    // Расчёт ускорения (второй производной)
    static Vector3D calculateAcceleration(const Vector3D& position) {
        double S = position.magnitude();
        if (S <= 1.0) return Vector3D();

        double aMagnitude = 1.0 / (S * S - 1.0);
        return position * (aMagnitude / S); // направление совпадает с позицией
    }
};

// Пример использования
int main() {
    // Начальная позиция и скорость
    Vector3D position(10.0, 0.0, 0.0);  // S ≈ 10
    Vector3D velocity(0.0, 1.0, 0.5);

    std::cout << "Позиция: (" << position.x << ", " << position.y << ", " << position.z << ")\n";
    std::cout << "Скорость: (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")\n\n";

    // Расчёт производных
    Vector3D jerk = PointDynamics::calculateJerk(position, velocity);
    Vector3D snap = PointDynamics::calculateSnap(position, velocity);

    std::cout << "Третья производная (рывок):\n";
    std::cout << "Jx = " << jerk.x << "\n";
    std::cout << "Jy = " << jerk.y << "\n";
    std::cout << "Jz = " << jerk.z << "\n\n";

    std::cout << "Четвёртая производная (snap):\n";
    std::cout << "Sx = " << snap.x << "\n";
    std::cout << "Sy = " << snap.y << "\n";
    std::cout << "Sz = " << snap.z << "\n";

    return 0;
}





static Vector3D calculateSnap(const Vector3D& position, const Vector3D& velocity) {
    double S = position.magnitude();
    if (S <= 1.0) {
        std::cerr << "Ошибка: S должно быть > 1 для корректного расчёта\n";
        return Vector3D();
    }

    double S_sq = S * S;
    double r_dot_v = position.dot(velocity);
    double dot_S = r_dot_v / S;  // dS/dt

    Vector3D acceleration = calculateAcceleration(position);
    double v_dot_v = velocity.dot(velocity);  // |v|^2
    double r_dot_a = position.dot(acceleration);  // r · a

    // Компонент A и его производная dA/dt
    double A = -2.0 * r_dot_v / ((S_sq - 1.0) * (S_sq - 1.0) * S);
    double numerator_A = (v_dot_v + r_dot_a) * (S_sq - 1.0) * S -
                   r_dot_v * (4.0 * S_sq * dot_S + (S_sq - 1.0) * dot_S);
    double denominator_A = (S_sq - 1.0) * (S_sq - 1.0) * S * S;
    double dA_dt = -2.0 * numerator_A / denominator_A;

    // Компонент B и его производная dB/dt
    double B = 1.0 / ((S_sq - 1.0) * S);
    double dB_dt = -(2.0 * S * dot_S * (S_sq - 1.0) * S +
               (S_sq - 1.0) * (S_sq - 1.0) * dot_S) /
               ((S_sq - 1.0) * (S_sq - 1.0) * S * S);

    // Вектор C и его производная dC/dt
    Vector3D C = position * (-r_dot_v / (S_sq * S));
    double factor_dC = (v_dot_v + r_dot_a) / (S * S * S) -
                 3.0 * r_dot_v * dot_S / (S * S * S);
    Vector3D dC_dt = velocity * factor_dC +
                   position * (-dot_S * r_dot_v / (S_sq * S) - r_dot_v * dot_S / (S_sq * S_sq));

    // Термы для расчёта snap
    Vector3D term1 = position * dA_dt;  // dA/dt * r
    Vector3D term2 = velocity * A;  // A * v
    Vector3D term3 = (velocity + C) * dB_dt;  // dB/dt * (v + C)
    Vector3D term4 = acceleration * B;  // B * a
    Vector3D term5 = dC_dt * B;  // B * dC/dt

    // Итоговая четвёртая производная
    Vector3D snap = term1 + term2 + term3 + term4 + term5;
    return snap;
}








*/
