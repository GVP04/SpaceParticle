#include "CommonFunc.h"

struct SPos
{
	double X;
	double Y;
	double Z;

	bool operator==(const SPos& other) const {
		return X == other.X && Y == other.Y && Z == other.Z;
	}
	bool operator!=(const SPos& other) const {
		return X != other.X || Y != other.Y || Z != other.Z;
	}

	SPos& operator+=(const SPos& other) {
		X += other.X;
		Y += other.Y;
		Z += other.Z;
		return *this;
	}

	SPos& operator-=(const SPos& other) {
		X -= other.X;
		Y -= other.Y;
		Z -= other.Z;
		return *this;
	}

	SPos& operator*=(const SPos& other) {
		X *= other.X;
		Y *= other.Y;
		Z *= other.Z;
		return *this;
	}
	SPos& operator/=(const SPos& other) {
		X /= other.X;
		Y /= other.Y;
		Z /= other.Z;
		return *this;
	}

	SPos& operator+=(double other) {
		X += other;
		Y += other;
		Z += other;
		return *this;
	}

	SPos& operator-=(double other) {
		X -= other;
		Y -= other;
		Z -= other;
		return *this;
	}

	SPos& operator*=(double other) {
		X *= other;
		Y *= other;
		Z *= other;
		return *this;
	}
	SPos& operator/=(double other) {
		double ttt = 1.0 / other;
		X *= ttt;
		Y *= ttt;
		Z *= ttt;
		return *this;
	}

	void SET(const SPos& in_Set)
	{
		X = in_Set.X;
		Y = in_Set.Y;
		Z = in_Set.Z;
	}

	void SET(const double in_X, const double in_Y, const double in_Z) 
	{
		X = in_X;
		Y = in_Y;
		Z = in_Z;
	}

	// Сложение векторов
	void ADD(const SPos& in_Add)
	{
		X += in_Add.X;
		Y += in_Add.Y;
		Z += in_Add.Z;
	}

	void ADD(const double in_Add)
	{
		X += in_Add;
		Y += in_Add;
		Z += in_Add;
	}

	// Вычитание векторов
	void MINUS(const SPos& in_MINUS)
	{
		X -= in_MINUS.X;
		Y -= in_MINUS.Y;
		Z -= in_MINUS.Z;
	}

	void MINUS(const double in_MINUS)
	{
		X -= in_MINUS;
		Y -= in_MINUS;
		Z -= in_MINUS;
	}

	void SET_AS_DELTA(const SPos& in_First, const SPos& in_Last)
	{
		X = in_First.X - in_Last.X;
		Y = in_First.Y - in_Last.Y;
		Z = in_First.Z - in_Last.Z;
	}

	void MULT(double in_Mult)
	{
		X *= in_Mult;
		Y *= in_Mult;
		Z *= in_Mult;
	}

	void DIV(double in_DIV)
	{
		if (in_DIV != 0.0)
		{
			X /= in_DIV;
			Y /= in_DIV;
			Z /= in_DIV;
		}
	}

	void INVERT()
	{
		X = -X;
		Y = -Y;
		Z = -Z;
	}

	void CUT(double in_MinLimit, double in_MaxLimit)
	{
		if (X < in_MinLimit) X = in_MinLimit;
		if (Y < in_MinLimit) Y = in_MinLimit;
		if (Z < in_MinLimit) Z = in_MinLimit;
		if (X > in_MaxLimit) X = in_MaxLimit;
		if (Y > in_MaxLimit) Y = in_MaxLimit;
		if (Z > in_MaxLimit) Z = in_MaxLimit;
	}

	void CUT(const SPos *in_MinLimit, const SPos* in_MaxLimit)
	{
		if (in_MinLimit)
		{
			if (X < in_MinLimit->X) X = in_MinLimit->X;
			if (Y < in_MinLimit->Y) Y = in_MinLimit->Y;
			if (Z < in_MinLimit->Z) Z = in_MinLimit->Z;
		}
		if (in_MaxLimit)
		{
			if (X > in_MaxLimit->X) X = in_MaxLimit->X;
			if (Y > in_MaxLimit->Y) Y = in_MaxLimit->Y;
			if (Z > in_MaxLimit->Z) Z = in_MaxLimit->Z;
		}
	}


	// Скалярное произведение
	inline double DOT(const SPos& in_DOT) const { return X * in_DOT.X + Y * in_DOT.Y + Z * in_DOT.Z;	}
	inline double DOT_ME() const { return X * X + Y * Y + Z * Z;	}

	// Векторное произведение
	inline void CROSS(const SPos& in_Vector)
	{
		double nX = Y * in_Vector.Z - Z * in_Vector.Y;
		double nY = Z * in_Vector.X - X * in_Vector.Z;
			    Z = X * in_Vector.Y - Y * in_Vector.X;
		X = nX;
		Y = nY;
	}

	// Векторное произведение
	SPos GETCROSS(const SPos& in_Vector) const
	{
		SPos ret;
		ret.X = Y * in_Vector.Z - Z * in_Vector.Y;
		ret.Y = Z * in_Vector.X - X * in_Vector.Z;
		ret.Z = X * in_Vector.Y - Y * in_Vector.X;

		return ret;
	}
	// Векторное произведение
	inline void SET_AS_CROSS(const SPos& in_Vector1, const SPos& in_Vector2)
	{
		X = in_Vector1.Y * in_Vector2.Z - in_Vector1.Z * in_Vector2.Y;
		Y = in_Vector1.Z * in_Vector2.X - in_Vector1.X * in_Vector2.Z;
		Z = in_Vector1.X * in_Vector2.Y - in_Vector1.Y * in_Vector2.X;
	}

	//длина от 0,0,0
	inline double LEN() const { return sqrt(X * X + Y * Y + Z * Z); }
	inline double LENPWR2() const { return X * X + Y * Y + Z * Z; }
	inline bool IsZero() const { return X == 0.0 && Y == 0.0 && Z == 0.0; }
	inline bool HaveZero() const { return X == 0.0 || Y == 0.0 || Z == 0.0; }
	inline double LENPWR3() const { double tmp = LEN();	return tmp * tmp * tmp; }

	static double GetDistance(const SPos& Ptr1, const SPos& Ptr2) { return sqrt(GetDistancePwr2(Ptr1, Ptr2)); }

	static double GetDistancePwr2(const SPos& Ptr1, const SPos& Ptr2)
	{
		double ret, tmp;
		tmp = Ptr1.X - Ptr2.X;
		ret = tmp * tmp;
		tmp = Ptr1.Y - Ptr2.Y;
		ret += tmp * tmp;
		tmp = Ptr1.Z - Ptr2.Z;
		ret += tmp * tmp;

		return ret;
	}

	inline double GetDistancePwr2(const SPos& Ptr1) const
	{
		double ret, tmp;
		tmp = Ptr1.X - X;
		ret = tmp * tmp;
		tmp = Ptr1.Y - Y;
		ret += tmp * tmp;
		tmp = Ptr1.Z - Z;
		ret += tmp * tmp;

		return ret;
	}
	inline double GetDistance(const SPos& Ptr1) const { return sqrt(GetDistancePwr2(Ptr1)); }

	void NORMALIZE() {
		double Len = LEN();
		if (Len != 0.0)
		{
			Len = 1.0 / Len;
			X *= Len;
			Y *= Len;
			Z *= Len;
		}
	}

	void SET_LENGHT(double in_Norm) {
		double Len = LEN();
		if (Len != 0.0)
		{
			Len = in_Norm / Len;
			X *= Len;
			Y *= Len;
			Z *= Len;
		}
	}

	void LIMIT_BY(double in_Limit) {
		double curLenPwr2 = LENPWR2();
		if (in_Limit * in_Limit  < curLenPwr2 && curLenPwr2 > 0.0)
		{
			curLenPwr2 = in_Limit / sqrt(curLenPwr2);
			X *= curLenPwr2;
			Y *= curLenPwr2;
			Z *= curLenPwr2;
		}
	}

	void LIMIT_BY_PWR2(double in_Limit) {
		double curLenPwr2 = LENPWR2();
		if (in_Limit < curLenPwr2 && curLenPwr2 > 0.0)
		{
			curLenPwr2 = sqrt(in_Limit / curLenPwr2);
			X *= curLenPwr2;
			Y *= curLenPwr2;
			Z *= curLenPwr2;
		}
	}

	void SET_AS_REFLECT(const SPos& FallVector, const SPos& Normal)
	{
		*this = FallVector;

		SPos n_norm = Normal;
		//n_norm.NORMALIZE();

		// Вычисляем скалярное произведение
		double dotProduct = DOT(n_norm);

		// Рассчитываем отраженный вектор
		n_norm.MULT(dotProduct + dotProduct);
		MINUS(n_norm);
	}

	void SET_AS_REFLECT(const SPos& Velocity, const SPos& planeVelocity, const SPos& Normal)
	{
		*this = Velocity;
		MINUS(planeVelocity);

		SPos n_norm = Normal;
		//n_norm.NORMALIZE();

		// Вычисляем скалярное произведение
		double dotProduct = DOT(n_norm);

		// Рассчитываем отраженный вектор

		n_norm.MULT(dotProduct + dotProduct);
		MINUS(n_norm);
		ADD(planeVelocity);
	}

	void REFLECT(const SPos& Normal)
	{
		// Нормализуем вектор нормали
		SPos n_norm = Normal;
		//n_norm.NORMALIZE();

		// Вычисляем проекцию скорости на нормаль
		double dot_product = DOT(n_norm);

		// Вычисляем параллельную компоненту
		SPos n_normTmp = n_norm;
		n_normTmp.MULT(dot_product);
		MINUS(n_normTmp);

		// Вычисляем перпендикулярную компоненту
		SPos v_perp = n_norm;
		v_perp.MULT(dot_product);

		// Отражение
		SPos v_perp_reflected = n_norm;
		v_perp_reflected.MULT(dot_product);
		v_perp_reflected.INVERT();

		// Итоговая скорость
		ADD(v_perp_reflected);
	}

	void REFLECT(const SPos& Normal, const SPos& planeVelocity)
	{
		MINUS(planeVelocity);

		SPos n_norm = Normal;
		//n_norm.NORMALIZE();

		// Вычисляем скалярное произведение
		double dotProduct = this->DOT(n_norm);

		// Рассчитываем отраженный вектор

		n_norm.MULT(dotProduct + dotProduct);
		MINUS(n_norm);
		ADD(planeVelocity);
	}


	void SET_AS_OPPOSITE(const SPos& v, const SPos& normal)
	{
		// Нормализуем вектор нормали
		SPos n_norm = normal;
		//n_norm.NORMALIZE();

		// Вычисляем проекцию на нормаль
		*this = v;
		n_norm.MULT(DOT(n_norm));

		// Находим параллельную компоненту
		MINUS(n_norm);

		// Нормализуем и меняем направление
		//NORMALIZE();
		INVERT();
	}

	void OPPOSITE(const SPos& normal) 
	{
		// Нормализуем вектор нормали
		SPos n_norm = normal;
		//n_norm.NORMALIZE();

		// Вычисляем проекцию на нормаль
		n_norm.MULT(DOT(n_norm));

		// Находим параллельную компоненту
		MINUS(n_norm);

		// Нормализуем и меняем направление
		NORMALIZE();
		INVERT();
	}

	//сделать средней точкой на прямой между двумя точками
	void SET_AS_MIDDLE_POINT(const SPos& in_Point1, const SPos& in_Point2)
	{
		X = (in_Point1.X + in_Point2.X) * 0.5;
		Y = (in_Point1.Y + in_Point2.Y) * 0.5;
		Z = (in_Point1.Z + in_Point2.Z) * 0.5;
	}

	//сделать средней пропорциональной точкой на прямой между двумя точками
	void SET_AS_MIDDLE_POINT(const SPos& in_Point1, const SPos& in_Point2, double Proportion)
	{
		X = (in_Point2.X - in_Point1.X) * Proportion + in_Point1.X;
		Y = (in_Point2.Y - in_Point1.Y) * Proportion + in_Point1.Y;
		Z = (in_Point2.Z - in_Point1.Z) * Proportion + in_Point1.Z;
	}

	void SET_AS_NORMAL(const SPos& in_Point1, const SPos& in_Point2, double in_Lenght = 1.0)
	{
		*this = in_Point1;
		MINUS(in_Point2);
		SET_LENGHT(in_Lenght);
	}

	void SET_AS_NORMAL( const SPos& in_Point2, double in_Lenght = 1.0)
	{
		MINUS(in_Point2);
		SET_LENGHT(in_Lenght);
	}


	void SET_AS_PROJECTION_OF(const SPos &in_SrcVector)
	{
		MULT(DOT(in_SrcVector) / LENPWR2());
	}

	void SET_AS_PROJECTION_OF_TO_NORMAL(const SPos& in_SrcVector)
	{
		MULT(DOT(in_SrcVector));
	}

	void SET_XYZ_AS(double in_Value) { X = Y = Z = in_Value; }


	double GET_PROJECTION_LENGHT_OF(const SPos &in_SrcVector) const
	{
		SPos tmp = *this;
		double dotRes = DOT(in_SrcVector);
		tmp.MULT(dotRes / LENPWR2());

		return  dotRes < 0.0 ? -tmp.LEN() : tmp.LEN();
	}

	double GET_PROJECTION_SIGN(const SPos& in_SrcVector) const
	{
		return DOT(in_SrcVector);
	}

	//сделать проекцией вектора(себя) на плоскость, если вектор и нормаль из одной точки на плоскости
	void SET_AS_PROJECTION_ON_PLANE(const SPos& in_PlaneNormal)
	{
		double LenNormPwr2 = in_PlaneNormal.LENPWR2();
		if (LenNormPwr2 != 0.0)
		{
			double tmpMltpl = DOT(in_PlaneNormal) / LenNormPwr2;
			X -= tmpMltpl * in_PlaneNormal.X;
			Y -= tmpMltpl * in_PlaneNormal.Y;
			Z -= tmpMltpl * in_PlaneNormal.Z;
		}
	}
	void SET_AS_PROJECTION_ON_PLANE_NORMAL(const SPos& in_PlaneNormal)
	{
			double tmpMltpl = DOT(in_PlaneNormal);
			X -= tmpMltpl * in_PlaneNormal.X;
			Y -= tmpMltpl * in_PlaneNormal.Y;
			Z -= tmpMltpl * in_PlaneNormal.Z;
	}
	//сделать проекцией точки на плоскость, если заданы точка на плоскости и нормаль из этой точки
	void SET_AS_PROJECTION_ON_PLANE(const SPos& point, const SPos& planePoint, const SPos& in_PlaneNormal)
	{
		// Вектор от точки плоскости до проецируемой точки
		*this = point;
		MINUS(planePoint);
		SET_AS_PROJECTION_ON_PLANE(in_PlaneNormal);
	}

	//void SET_AS_PROJECTION_ON_PLANE__(const SPos& point, const SPos& planePoint, const SPos& in_PlaneNormal)
	//{
	//	//вычисляется по формуле : D = -(A * x0 + B * y0 + C * z0), где(x0, y0, z0) — координаты точки на плоскости.
	//	//double D = -in_PlaneNormal.DOT(planePoint);  //не используется здесь
	//	// Скалярное произведение вектора point на нормаль
	//	//double dotProduct = point.DOT(in_PlaneNormal);
	//	// Скалярное произведение нормали на себя
	//	//double normSquared = in_PlaneNormal.DOT_ME(); //==LENPWR2
	//	//projOnNormal.MULT(dotProduct / normSquared);


	//	// Проекция вектора на нормаль
	//	SPos projOnNormal = in_PlaneNormal;
	//	projOnNormal.MULT(point.DOT(in_PlaneNormal) / in_PlaneNormal.DOT_ME());

	//	// Проекция вектора на плоскость
	//	*this = point;
	//	MINUS(projOnNormal);
	//}



	//сделать прямой, являющейся пересечением двух плоскостей. Нормали из одной точки, лежащей на этой прямой
	void SET_AS_CROSS_OF_PLANES(const SPos& in_PlaneNormal1, const SPos& in_PlaneNormal2)
	{
		X = in_PlaneNormal1.Y * in_PlaneNormal2.Z - in_PlaneNormal1.Z * in_PlaneNormal2.Y;
		Y = in_PlaneNormal1.Z * in_PlaneNormal2.X - in_PlaneNormal1.X * in_PlaneNormal2.Z;
		Z = in_PlaneNormal1.X * in_PlaneNormal2.Y - in_PlaneNormal1.Y * in_PlaneNormal2.X;
	}

	//сделать прямой, являющейся пересечением двух плоскостей. Нормали из произвольных точек
	SPos SET_AS_CROSS_OF_PLANES(const SPos& in_PlaneNormal1, const SPos& point1, const SPos& in_PlaneNormal2, const SPos& point2)
	{
		SPos intersectionPoint;
		// Находим направляющий вектор как векторное произведение нормалей
		SET_AS_CROSS(in_PlaneNormal1, in_PlaneNormal2);

		if (!IsZero())
		{
			// Решаем систему уравнений для нахождения точки пересечения
			// Используем метод подстановки
			double a1 = in_PlaneNormal1.X, b1 = in_PlaneNormal1.Y, c1 = in_PlaneNormal1.Z;
			double d1 = -in_PlaneNormal1.DOT(point1);

			double a2 = in_PlaneNormal2.X, b2 = in_PlaneNormal2.Y, c2 = in_PlaneNormal2.Z;
			double d2 = -in_PlaneNormal2.DOT(point2);

			// Находим точку пересечения
			double det = a1 * b2 - a2 * b1;

			if (fabs(det) < 1.0e-9) {
				// Если определитель мал, меняем стратегию
				det = a1 * c2 - a2 * c1;
				if (fabs(det) < 1.0e-9) {
					det = b1 * c2 - b2 * c1;
				}
			}

			det = 1.0 / det;
			// Вычисляем координаты точки
			intersectionPoint.X = (b1 * d2 - b2 * d1) * det;
			intersectionPoint.Y = (a2 * d1 - a1 * d2) * det;

			if (fabs(c1) > fabs(c2)) 
				intersectionPoint.Z = (d1 - a1 * intersectionPoint.X - b1 * intersectionPoint.Y) / c1;
			else 
				intersectionPoint.Z = (d2 - a2 * intersectionPoint.X - b2 * intersectionPoint.Y) / c2;
		}
		else
			intersectionPoint = { 0 };

		return intersectionPoint;

	}

	static int PRINT_AS_NULL(char* io_Str, int in_StrLen, int in_ValueDelim = '\t')
	{
		int ret = 0;
		if (io_Str)
		{
			for (int i = 0; i < 3 && in_StrLen - ret > 0; i++)
				if (in_StrLen - (ret += sprintf_s(io_Str + ret, in_StrLen - ret, "nullptr")) > 0 && i < 2)
					io_Str[ret++] = in_ValueDelim;

			io_Str[ret] = 0;
		}
		return ret;
	}

	int PRINT(char* io_Str, int in_StrLen, int in_ValueDelim = '\t', const char* in_Format = "%g") const
	{
		int ret = 0;
		if (io_Str)
		{
			if (in_Format && *in_Format)
			{
				const double* pData = &X;

				for (int i = 0; i < 3 && in_StrLen - ret > 0; i++, pData++)
					if (in_StrLen - (ret += sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, *pData)) > 0 && i < 2)
						io_Str[ret++] = in_ValueDelim;
			}
			io_Str[ret] = 0;
		}
		return ret;
	}

};

struct SPosI64
{
	__int64 X;
	__int64 Y;
	__int64 Z;
};

union SPosUnion
{
	SPos posDbl;
	SPosI64 posInt64;
};

struct SData_SD
{
	double Position;
	double Velocity;
	double Accel_2_3D;
	double Jerk_3_3D;
	double AbsSpeed;
	double Density;
	double TimePoint; // типа, это время
};


#define SDATA_ALL			0x070F

#define SDATA_ALL_VECTORS	0x000F
#define SDATA_POSITION		0x0001
#define SDATA_SPEED			0x0002
#define SDATA_ACCEL3D		0x0004
#define SDATA_ACCELDERIV3D	0x0008

#define SDATA_ALL_SCALARS	0x0700
#define SDATA_ABSSPEED		0x0100
#define SDATA_DENSITY		0x0200
#define SDATA_TIMEPOINT		0x0400

/*
0	Position	Расстояние, положение	x(t)	м(m)
1	Velocity	Скорость	v(t),x˙м / с(m / s)
2	Acceleration	Ускорение	a(t),x¨м / с²(m / s²)
3	Jerk	Рывок	j(t), x(3)м / с³(m / s³)
4	Snap / Jounce	Рывок рывка	s(t), x(4)м / с⁴(m / s⁴)
5	Crackle	Пятая производная	c(t), x(5)м / с⁵(m / s⁵)
6	Pop	Шестая производная	p(t), x(6)м / с⁶(m / s⁶)
*/


struct SData
{
	SPos Position;
	SPos Velocity;
	SPos Accel_2_3D;
	SPos Jerk_3_3D;
	//SPos Snap_4_3D;
	//SPos Crackle_5_3D;
	//SPos Pop_6_3D;
	double AbsSpeed;
	double Density;
	double TimePoint; // типа, это момент времени

	void CLEAR()
	{
		Position = { 0 };
		Velocity = { 0 };
		Accel_2_3D = { 0 };
		Jerk_3_3D = { 0 };
		AbsSpeed = 0.0;
		Density = 0.0;
		TimePoint = -9.0e99;
	};

	void SET_AS_FARAWAY()
	{
		CLEAR();
		Position.X = Position.Y = Position.Z = 9.9e99;
		TimePoint = -9.0e99; 
	}

	bool IS_FARAWAY() const
	{
		return TimePoint < -1.0e10 || Position.X > 1.0e10; // || Position.Y > 1.0e10 || Position.Z > 1.0e10;
	}

	static int PRINT_AS_NULL(char* io_Str, int in_StrLen, int in_Flags, int in_ValueDelim = '\t', int in_SubValueDelim = '\t') 
	{
		int ret = 0;
		if (io_Str)
		{
			const char* txtNULL = "nullptr";
			int nOutput;
			if (in_StrLen - ret > 0 && (in_Flags & SDATA_POSITION))
			{
				in_Flags ^= SDATA_POSITION;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Position");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
						io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_SPEED))
			{
				in_Flags ^= SDATA_SPEED;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Velocity");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCEL3D))
			{
				in_Flags ^= SDATA_ACCEL3D;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Accel_2_3D");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCELDERIV3D))
			{
				in_Flags ^= SDATA_ACCELDERIV3D;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Jerk_3_3D");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0  && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_ABSSPEED))
			{
				in_Flags ^= SDATA_ABSSPEED;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "AbsSpeed");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, txtNULL);
				if (in_StrLen - (ret += nOutput) > 0  && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_DENSITY))
			{
				in_Flags ^= SDATA_DENSITY;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Density");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, txtNULL);
				if (in_StrLen - (ret += nOutput) > 0  && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_TIMEPOINT))
			{
				in_Flags ^= SDATA_TIMEPOINT;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "TimePoint");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, txtNULL);
				if (in_StrLen - (ret += nOutput) > 0  && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}
			*io_Str = 0;
		}

		return ret;
	}

	int PRINT(char* io_Str, int in_StrLen, int in_Flags, int in_ValueDelim = '\t', int in_SubValueDelim = '\t', const char* in_Format = "%g") const
	{
		int ret = 0;
		if (io_Str)
		{
			if (in_Format && *in_Format)
			{
				int nOutput;
				if (in_StrLen - ret > 0 && (in_Flags & SDATA_POSITION))
				{
					in_Flags ^= SDATA_POSITION;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Position");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;

					nOutput = Position.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_SPEED))
				{
					in_Flags ^= SDATA_SPEED;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Velocity");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = Velocity.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCEL3D))
				{
					in_Flags ^= SDATA_ACCEL3D;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Accel_2_3D");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = Accel_2_3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCELDERIV3D))
				{
					in_Flags ^= SDATA_ACCELDERIV3D;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Jerk_3_3D");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = Jerk_3_3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_ABSSPEED))
				{
					in_Flags ^= SDATA_ABSSPEED;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "AbsSpeed");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, AbsSpeed);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_DENSITY))
				{
					in_Flags ^= SDATA_DENSITY;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Density");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, Density);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)		io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_TIMEPOINT))
				{
					in_Flags ^= SDATA_TIMEPOINT;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "TimePoint");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, in_Format, TimePoint);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)		io_Str[ret++] = in_ValueDelim;
				}
			}
			io_Str[ret] = 0;
		}

		return ret;
	};

	bool IsSlowdown() const
	{
		return Velocity.GET_PROJECTION_SIGN(Accel_2_3D) < 0.0;
	}

	double GetRelativeSpeed(const SData& Ptr1, SPos* const RSpeed3D) const { return  SData::GetRelativeSpeed(*this, Ptr1, RSpeed3D); }
	static double GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2, SPos* const RSpeed3D) 
	{
		double ret = 0.0;
		SPos distanceVector = Ptr2.Position; 
		distanceVector.MINUS(Ptr1.Position);

		SPos velocityVector = Ptr2.Velocity;
		velocityVector.MINUS(Ptr1.Velocity);

		if (RSpeed3D)
			*RSpeed3D = velocityVector;

		ret = distanceVector.GET_PROJECTION_LENGHT_OF(velocityVector);

		return ret;
	}

#define LINEARMOVECLEARFLAG_NONE			0x0000
#define LINEARMOVECLEARFLAG_SPEED			0x0001
#define LINEARMOVECLEARFLAG_ACCEL3D			0x0002
#define LINEARMOVECLEARFLAG_ACCELDERIV3D	0x0004

	void LinearMove(double in_Time, int in_ClearFlag = LINEARMOVECLEARFLAG_NONE)
	{
		SPos delta = Velocity;
		delta.MULT(in_Time);
		Position.ADD(delta);
		TimePoint += in_Time;
		if (in_ClearFlag)
		{
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_SPEED))	Velocity = { 0 };
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_ACCEL3D))	Accel_2_3D = { 0 };
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_ACCELDERIV3D))	Jerk_3_3D = { 0 };
		}
	}

	int SetLinearFrom(const SData& Data1, const SData& Data2, double in_Time)
	{
		int ret = -1;

		if (in_Time > Data1.TimePoint)
		{
			if (in_Time < Data2.TimePoint)
			{
				ret = 0;
				double mltpl = (in_Time - Data1.TimePoint) / (Data2.TimePoint - Data1.TimePoint);
				Position.SET_AS_MIDDLE_POINT(Data1.Position, Data2.Position, mltpl);
				Velocity.SET_AS_MIDDLE_POINT(Data1.Velocity, Data2.Velocity, mltpl);
				Accel_2_3D.SET_AS_MIDDLE_POINT(Data1.Accel_2_3D, Data2.Accel_2_3D, mltpl);
				Jerk_3_3D.SET_AS_MIDDLE_POINT(Data1.Jerk_3_3D, Data2.Jerk_3_3D, mltpl);
				//SPos Snap_4_3D;
				//SPos Crackle_5_3D;
				//SPos Pop_6_3D;

				TimePoint = in_Time;

				AbsSpeed = (Data2.AbsSpeed - Data1.AbsSpeed) * mltpl + Data1.AbsSpeed;
				Density = (Data2.Density - Data1.Density) * mltpl + Data1.Density;
			}
			else
				if (in_Time == Data2.TimePoint)
					*this = Data2;
		}
		else
			if (in_Time == Data1.TimePoint)
				*this = Data1;

		return ret;
	}

	//вычисление точки перехода на скорость in_AbsSpeed. ускорение === const
	double GetTimeToSpeed(double in_AbsSpeed) const
	{
		//double A = Accel_2_3D.LENPWR2();
		double B = Accel_2_3D.DOT(Velocity); B += B;
		//double C = Velocity.LENPWR2() - in_AbsSpeed*in_AbsSpeed;

		TwoDoubleRoots result = QuadraticEequationSolution(/*A*/Accel_2_3D.LENPWR2(), B, /*C*/Velocity.LENPWR2() - in_AbsSpeed* in_AbsSpeed);

		double ret = result.root1.Dbl;
		if (isnan(ret) || ret < 0.0)
		{
			ret = result.root2.Dbl;
			if (isnan(ret) || ret < 0.0)
				ret = 0x7FF8000000000000ULL;
		}
		return ret;
	}

	//вычисление точки перехода на скорость in_AbsSpeed. ускорение === const
	double GetTimeToSpeedPwr2(double in_AbsSpeedPwr2) const
	{
		//double A = Accel_2_3D.LENPWR2();
		double B = Accel_2_3D.DOT(Velocity); B += B;
		//double C = Velocity.LENPWR2() - in_AbsSpeed;

		TwoDoubleRoots result = QuadraticEequationSolution(/*A*/Accel_2_3D.LENPWR2(), B, /*C*/Velocity.LENPWR2() - in_AbsSpeedPwr2);

		double ret = result.root1.Dbl;
		if (isnan(ret) || ret < 0.0)
		{
			ret = result.root2.Dbl;
			if (isnan(ret) || ret < 0.0)
				ret = 0x7FF8000000000000ULL;
		}
		return ret;
	}

	bool TAD_IsToSquize(const SData* PrevPrevData, double SqueezeStep, int CompareFlag) const
	{
		//CompareFlag не используется здесь. А так можно по флагу определить какие переменные сравнивать
		return TimePoint - PrevPrevData->TimePoint < SqueezeStep; //если разница больше 
	}


	static bool TAD_CorrectData(SData& PrevPrevData, const SData& PrevData, int CorrectionFlag = 0)
	{
		double DeltaT = PrevData.TimePoint - PrevPrevData.TimePoint;
		if (DeltaT != 0.0)
		{
			DeltaT = 1.0 / DeltaT;
			PrevPrevData.Jerk_3_3D.SET_AS_DELTA(PrevData.Accel_2_3D, PrevPrevData.Accel_2_3D);
			PrevPrevData.Jerk_3_3D.MULT(DeltaT);
			//PrevPrevData.Jerk_3_3D = { 0 };
			ASSERT(fabs(PrevPrevData.Jerk_3_3D.X) < 1.0e80);

			//////Snap_4_3D

			//F_3(T) = (S2 - S1) * (t-t1)^2/2/(t2-t1) + s1*(t-t1) + (A2-A1)/(t2-t1) - (S2+2*S1)*(t2-t1)/6
			//F_3(T==T1) = (A2-A1)/(t2-t1) - (S2+2*S1)*(t2-t1)/6
			//F_3(T==T1) = (A2-A1)/DeltaT - (S2+S1+S1)*DeltaT/6
			//PrevPrevData.Jerk_3_3D.SET_AS_DELTA(PrevData.Accel_2_3D, PrevPrevData.Accel_2_3D);
			//PrevPrevData.Jerk_3_3D.MULT(DeltaT);
		}
		else
			PrevPrevData.Jerk_3_3D = { 0 };


		return true; //если разница больше 
	}
};

#define READPARAMS_NOT_USE_DEFS		0x1
#define READPARAMS_USE_DEFS			0x0


int ReadCalcParam(char* in_Src, const char* in_Mask, SPos& data, double defValu, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(char* in_Src, const char* in_Mask, int& data, int defValue, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(char* in_Src, const char* in_Mask, double& data, double defValue, int inFlags = READPARAMS_USE_DEFS);
