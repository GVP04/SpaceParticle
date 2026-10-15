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

	inline void SET_AS_SUMM(const SPos& in_Vector1, const SPos& in_Vector2)
	{
		X = in_Vector1.X + in_Vector2.X;
		Y = in_Vector1.Y + in_Vector2.Y;
		Z = in_Vector1.Z + in_Vector2.Z;
	}

	//суммирует и нормализует результат
	inline void SET_AS_SUMM_EQ1(const SPos& in_Vector1, const SPos& in_Vector2)
	{
		X = in_Vector1.X + in_Vector2.X;
		Y = in_Vector1.Y + in_Vector2.Y;
		Z = in_Vector1.Z + in_Vector2.Z;

		double LenPwr2 = 1.0 / sqrt(X * X + Y * Y + Z * Z);
		X *= LenPwr2;
		Y *= LenPwr2;
		Z *= LenPwr2;

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

	inline void SET_AS_MULT(const SPos& in_Vector1, double in_Mult)
	{
		X = in_Vector1.X * in_Mult;
		Y = in_Vector1.Y * in_Mult;
		Z = in_Vector1.Z * in_Mult;
	}

	inline void ADDIntegralStep(const SPos& Derivative, double StepValue)
	{
		X += Derivative.X * StepValue;
		Y += Derivative.Y * StepValue;
		Z += Derivative.Z * StepValue;
	}

	inline void ADDIntegralStep(const SPos& Derivative, const SPos& Derivative2, double StepValue)
	{
		double mltpl = StepValue * StepValue * 0.5;
		X += Derivative.X * StepValue + Derivative2.X * mltpl;
		Y += Derivative.Y * StepValue + Derivative2.Y * mltpl;
		Z += Derivative.Z * StepValue + Derivative2.Z * mltpl;
	}


	//сделать вектор суммой CurrentValue и произведения Derivative на StepValue
	inline void SET_AS_IntegralOf(const SPos& CurrentValue, const SPos& Derivative, double StepValue)
	{
		X = CurrentValue.X + Derivative.X * StepValue;
		Y = CurrentValue.Y + Derivative.Y * StepValue;
		Z = CurrentValue.Z + Derivative.Z * StepValue;
	}

	inline void SET_AS_IntegralOf(const SPos& CurrentValue, const SPos& Derivative, const SPos& Derivative2, double StepValue)
	{
		X = CurrentValue.X + Derivative.X * StepValue;
		Y = CurrentValue.Y + Derivative.Y * StepValue;
		Z = CurrentValue.Z + Derivative.Z * StepValue;
		StepValue = StepValue * StepValue * 0.5;
		X += Derivative2.X * StepValue;
		Y += Derivative2.Y * StepValue;
		Z += Derivative2.Z * StepValue;
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

	//вектор из in_Point1, ортогональный плоскости трех точек in_Point1, in_Point2, in_Point3
	inline void SET_AS_NORMAL_TO_PLANE_FROM_1(const SPos& in_Point1, const SPos& in_Point2, const SPos& in_Point3)
	{
		SPos Delta_12;
		Delta_12.X = in_Point1.X - in_Point2.X;
		Delta_12.Y = in_Point1.Y - in_Point2.Y;
		Delta_12.Z = in_Point1.Z - in_Point2.Z;

		SPos Delta_13;
		Delta_13.X = in_Point1.X - in_Point3.X;
		Delta_13.Y = in_Point1.Y - in_Point3.Y;
		Delta_13.Z = in_Point1.Z - in_Point3.Z;

		X = Delta_12.Y * Delta_13.Z - Delta_12.Z * Delta_13.Y + in_Point1.X;
		Y = Delta_12.Z * Delta_13.X - Delta_12.X * Delta_13.Z + in_Point1.Y;
		Z = Delta_12.X * Delta_13.Y - Delta_12.Y * Delta_13.X + in_Point1.Z;
	}

	//вектор из 0,0,0, ортогональный плоскости трех точек in_Point1, in_Point2, in_Point3
	inline void SET_AS_NORMAL_TO_PLANE_FROM_0(const SPos& in_Point1, const SPos& in_Point2, const SPos& in_Point3)
	{
		SPos Delta_12;
		Delta_12.X = in_Point1.X - in_Point2.X;
		Delta_12.Y = in_Point1.Y - in_Point2.Y;
		Delta_12.Z = in_Point1.Z - in_Point2.Z;

		SPos Delta_13;
		Delta_13.X = in_Point1.X - in_Point3.X;
		Delta_13.Y = in_Point1.Y - in_Point3.Y;
		Delta_13.Z = in_Point1.Z - in_Point3.Z;

		X = Delta_12.Y * Delta_13.Z - Delta_12.Z * Delta_13.Y;
		Y = Delta_12.Z * Delta_13.X - Delta_12.X * Delta_13.Z;
		Z = Delta_12.X * Delta_13.Y - Delta_12.Y * Delta_13.X;
	}

	//вектор из 0,0,0, ортогональный плоскости трех точек this, in_Point2, in_Point3
	inline void SET_AS_NORMAL_TO_PLANE_FROM_0(const SPos& in_Point2, const SPos& in_Point3)
	{
		SPos Delta_12;
		Delta_12.X = X - in_Point2.X;
		Delta_12.Y = Y - in_Point2.Y;
		Delta_12.Z = Z - in_Point2.Z;

		SPos Delta_13;
		Delta_13.X = X - in_Point3.X;
		Delta_13.Y = Y - in_Point3.Y;
		Delta_13.Z = Z - in_Point3.Z;

		X = Delta_12.Y * Delta_13.Z - Delta_12.Z * Delta_13.Y;
		Y = Delta_12.Z * Delta_13.X - Delta_12.X * Delta_13.Z;
		Z = Delta_12.X * Delta_13.Y - Delta_12.Y * Delta_13.X;
	}

	//установить как вектор из this, ортогональный плоскости трех точек this, in_Point2, in_Point3
	inline void SET_AS_NORMAL_TO_PLANE_FROM_this(const SPos& in_Point2, const SPos& in_Point3)
	{
		SPos Delta_12;
		Delta_12.X = X - in_Point2.X;
		Delta_12.Y = Y - in_Point2.Y;
		Delta_12.Z = Z - in_Point2.Z;

		SPos Delta_13;
		Delta_13.X = X - in_Point3.X;
		Delta_13.Y = Y - in_Point3.Y;
		Delta_13.Z = Z - in_Point3.Z;

		X = Delta_12.Y * Delta_13.Z - Delta_12.Z * Delta_13.Y + X;
		Y = Delta_12.Z * Delta_13.X - Delta_12.X * Delta_13.Z + Y;
		Z = Delta_12.X * Delta_13.Y - Delta_12.Y * Delta_13.X + Z;
	}


	//длина от 0,0,0
	inline double LEN() const { return sqrt(X * X + Y * Y + Z * Z); }
	inline double LENPWR2() const { return X * X + Y * Y + Z * Z; }
	inline bool IsZero() const { return X == 0.0 && Y == 0.0 && Z == 0.0; }
	inline bool HaveZero() const { return X == 0.0 || Y == 0.0 || Z == 0.0; }
	inline double LENPWR3() const { double tmp = LEN();	return tmp * tmp * tmp; }
	inline bool IsCollinear(const SPos& cmpSPos) const { return Y * cmpSPos.Z - Z * cmpSPos.Y == 0.0 && Z * cmpSPos.X - Z * cmpSPos.Z == 0.0 && X * cmpSPos.Y - Y * cmpSPos.X == 0.0; }




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
	//==NORMALIZE()
	void SET_LENGHT_TO_1()
	{
		double Len = LEN();
		if (Len != 0.0)
		{
			Len = 1.0 / Len;
			X *= Len;
			Y *= Len;
			Z *= Len;
		}
	}
	void SET_LENGHT(double in_Norm) 
	{//3 +* * 1 / sqrt
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

	void SET_AS_REFLECT(const SPos& ParticleVelocity, const SPos& planeVelocity, const SPos& Normal)
	{
		*this = ParticleVelocity;
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

	void SET_AS_NORMAL_VECTOR(const SPos& in_Point1, const SPos& in_Point2, double in_Lenght = 1.0)
	{
		*this = in_Point1;
		MINUS(in_Point2);
		SET_LENGHT(in_Lenght);
	}

	void SET_AS_NORMAL_VECTOR( const SPos& in_Point2, double in_Lenght = 1.0)
	{
		MINUS(in_Point2);
		SET_LENGHT(in_Lenght);
	}

	double SET_AS_PROJECTION_OF(const SPos &in_SrcVector)
	{
		double ret_Dot;
		MULT((ret_Dot = DOT(in_SrcVector)) / LENPWR2());
		return ret_Dot;
	}

	double SET_AS_PROJECTION_ONTO_LINE(const SPos& in_DstVector)
	{
		double ret_Dot = DOT(in_DstVector);
		SET_AS_MULT(in_DstVector, ret_Dot / in_DstVector.LENPWR2());
		return ret_Dot;
	}


	void SET_AS_PROJECTION_OF_TO_R1(const SPos& in_SrcVector)
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

	//смещает точку на расстояние от другой точки по линни этих точек
	void MOVE_TO_DISTANCE_FROM(const SPos& pointFrom, double distance)
	{
		// Вектор от точки плоскости до проецируемой точки
		MINUS(pointFrom);
		SET_LENGHT(distance);
		ADD(pointFrom);
	}


	//сделать проекцией вектора(себя) на плоскость, если вектор и нормаль из одной точки на плоскости
	void SET_AS_PROJECTION_ONTO_PLANE(const SPos& in_PlaneNormal)
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
	void SET_AS_PROJECTION_ONTO_PLANE(const SPos& in_PlaneNormal, double in_PlaneNormalLenPwr2)
	{//3 *+*-   1 /
		if (in_PlaneNormalLenPwr2 != 0.0)
		{
			double tmpMltpl = DOT(in_PlaneNormal) / in_PlaneNormalLenPwr2;
			X -= tmpMltpl * in_PlaneNormal.X;
			Y -= tmpMltpl * in_PlaneNormal.Y;
			Z -= tmpMltpl * in_PlaneNormal.Z;
		}
	}
	void SET_AS_PROJECTION_ONTO_PLANE_R1(const SPos& in_PlaneNormal)
	{
			double tmpMltpl = DOT(in_PlaneNormal);
			X -= tmpMltpl * in_PlaneNormal.X;
			Y -= tmpMltpl * in_PlaneNormal.Y;
			Z -= tmpMltpl * in_PlaneNormal.Z;
	}
	//сделать проекцией точки на плоскость, если заданы точка на плоскости и нормаль из этой точки
	void SET_AS_PROJECTION_ONTO_PLANE(const SPos& point, const SPos& planePoint, const SPos& in_PlaneNormal)
	{
		// Вектор от точки плоскости до проецируемой точки
		*this = point;
		MINUS(planePoint);
		SET_AS_PROJECTION_ONTO_PLANE(in_PlaneNormal);
	}

	double PERIMETR() const
	{
		return fabs(X) + fabs(Y) + fabs(Z);
	}

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
	SPos Velocity_Linear;

	//SPos Snap_4_3D;
	//SPos Crackle_5_3D;
	//SPos Pop_6_3D;
	double AbsSpeed;
	double Density;
	double TimePoint; // типа, это момент времени
	int State;
	int Reserve;

	void CLEAR()
	{
		Position = { 0 };
		Velocity = { 0 };
		Accel_2_3D = { 0 };
		Jerk_3_3D = { 0 };
		Velocity_Linear = { 0 };
		AbsSpeed = 0.0;
		Density = 0.0;
		TimePoint = 0.0;
		State = 0;
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

	void SET_Velocity()
	{
		Velocity_Linear = Velocity;
		AbsSpeed = Velocity.LEN();
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

	double GetRelativeSpeedFromVelocity(const SData& Ptr1, SPos* const RSpeed3D) const { return  SData::GetRelativeSpeedFromVelocity(*this, Ptr1, RSpeed3D); }
	static double GetRelativeSpeedFromVelocity(const SData& Ptr1, const SData& Ptr2, SPos* const RSpeed3D) 
	{// < 0 сближение
		double ret = 0.0;
		SPos distanceVector; distanceVector.SET_AS_DELTA(Ptr2.Position, Ptr1.Position);
		SPos velocityVector = Ptr2.Velocity; velocityVector.SET_AS_DELTA(Ptr2.Velocity, Ptr1.Velocity);

		ret = distanceVector.GET_PROJECTION_LENGHT_OF(velocityVector);
		if (isnan(ret)) ret = 0.0;
		if (RSpeed3D)
		{
			*RSpeed3D = velocityVector;
			RSpeed3D->SET_LENGHT(fabs(ret));
		}

		return ret;
	}

	static double GetRelativeSpeedFromVelocity(const SData& Ptr1, const SPos& StablePoint, SPos* const RSpeed3D)
	{// < 0 сближение
		double ret = 0.0;
		SPos distanceVector; distanceVector.SET_AS_DELTA(StablePoint, Ptr1.Position);

		ret = distanceVector.GET_PROJECTION_LENGHT_OF(Ptr1.Velocity);
		if (isnan(ret)) ret = 0.0;
		if (RSpeed3D)
		{
			*RSpeed3D = Ptr1.Velocity;
			RSpeed3D->SET_LENGHT(fabs(ret));
		}

		return ret;
	}

	double GetRelativeSpeedFromVelocity_Linear(const SData& Ptr1, SPos* const RSpeed3D) const 
	{
		return  SData::GetRelativeSpeedFromVelocity_Linear(*this, Ptr1, RSpeed3D); 
	}

	static double GetRelativeSpeedFromVelocity_Linear(const SData& Ptr1, const SData& Ptr2, SPos* const RSpeed3D)
	{// < 0 сближение
		double ret = 0.0;
		SPos distanceVector; distanceVector.SET_AS_DELTA(Ptr2.Position, Ptr1.Position);

		SPos velocityVector; velocityVector.SET_AS_DELTA(Ptr2.Velocity_Linear, Ptr1.Velocity_Linear);

		ret = distanceVector.GET_PROJECTION_LENGHT_OF(velocityVector);
		if (isnan(ret)) ret = 0.0;
		if (RSpeed3D)
		{
			*RSpeed3D = velocityVector;
			RSpeed3D->SET_LENGHT(fabs(ret));
		}

		return ret;
	}

	static double GetRelativeSpeedFromVelocity_Linear(const SData& Ptr1, const SPos& StablePoint, SPos* const RSpeed3D)
	{// < 0 сближение
		double ret = 0.0;
		SPos distanceVector; distanceVector.SET_AS_DELTA(StablePoint, Ptr1.Position);

		ret = distanceVector.GET_PROJECTION_LENGHT_OF(Ptr1.Velocity_Linear);
		if (isnan(ret)) ret = 0.0;
		if (RSpeed3D)
		{
			*RSpeed3D = Ptr1.Velocity_Linear;
			RSpeed3D->SET_LENGHT(fabs(ret));
		}

		return ret;
	}

#define LINEARMOVECLEARFLAG_NONE			0x0000
#define LINEARMOVECLEARFLAG_SPEED			0x0001
#define LINEARMOVECLEARFLAG_ACCEL3D			0x0002
#define LINEARMOVECLEARFLAG_ACCELDERIV3D	0x0004

	void LinearMove(double in_Time, int in_ClearFlag = LINEARMOVECLEARFLAG_NONE)
	{
		Position.ADDIntegralStep(Velocity_Linear, in_Time);
		TimePoint += in_Time;
		if (in_ClearFlag)
		{
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_SPEED))	Velocity = { 0 };
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_ACCEL3D))	Accel_2_3D = { 0 };
			if ((in_ClearFlag & LINEARMOVECLEARFLAG_ACCELDERIV3D))	Jerk_3_3D = { 0 };
		}
	}

	int SetLinearFromVelosity(const SData& Data1, const SData& Data2, double in_Time)
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
				Velocity_Linear = Velocity;
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

	int SetLinearFromVelocity_Linear(const SData& Data1, const SData& Data2, double in_Time)
	{
		int ret = -1;

		if (in_Time > Data1.TimePoint)
		{
			if (in_Time < Data2.TimePoint)
			{
				ret = 0;
				double mltpl = (in_Time - Data1.TimePoint) / (Data2.TimePoint - Data1.TimePoint);
				Position.SET_AS_MIDDLE_POINT(Data1.Position, Data2.Position, mltpl);
				Velocity.SET_AS_MIDDLE_POINT(Data1.Velocity_Linear, Data2.Velocity_Linear, mltpl);
				Velocity_Linear = Velocity;
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
	double GetTimeToSpeedByVelocity(double in_AbsSpeed) const
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
	double GetTimeToSpeedPwr2ByVelocity(double in_AbsSpeedPwr2) const
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

	//вычисление точки перехода на скорость in_AbsSpeed. ускорение === const
	double GetTimeToSpeedByVelocity_Linear(double in_AbsSpeed) const
	{
		//double A = Accel_2_3D.LENPWR2();
		double B = Accel_2_3D.DOT(Velocity_Linear); B += B;
		//double C = Velocity_Linear.LENPWR2() - in_AbsSpeed*in_AbsSpeed;

		TwoDoubleRoots result = QuadraticEequationSolution(/*A*/Accel_2_3D.LENPWR2(), B, /*C*/Velocity_Linear.LENPWR2() - in_AbsSpeed* in_AbsSpeed);

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
	double GetTimeToSpeedPwr2ByVelocity_Linear(double in_AbsSpeedPwr2) const
	{
		//double A = Accel_2_3D.LENPWR2();
		double B = Accel_2_3D.DOT(Velocity_Linear); B += B;
		//double C = Velocity_Linear.LENPWR2() - in_AbsSpeed;

		TwoDoubleRoots result = QuadraticEequationSolution(/*A*/Accel_2_3D.LENPWR2(), B, /*C*/Velocity_Linear.LENPWR2() - in_AbsSpeedPwr2);

		double ret = result.root1.Dbl;
		if (isnan(ret) || ret < 0.0)
		{
			ret = result.root2.Dbl;
			if (isnan(ret) || ret < 0.0)
				ret = 0x7FF8000000000000ULL;
		}
		return ret;
	}

	bool TAD_OnAdd(SData* PrevData)
	{
		PrevData->Velocity_Linear.SET_AS_DELTA(Position, PrevData->Position);
		double dTime = TimePoint - PrevData->TimePoint;
		if (dTime > 0.0)
		{
			PrevData->Velocity_Linear /= dTime;
			if (PrevData->Velocity_Linear.LEN() > 1.0)
				PrevData->Velocity_Linear.SET_LENGHT_TO_1();
		}
		else
			PrevData->Velocity_Linear = PrevData->Velocity;


		Velocity_Linear = Velocity;

		ASSERT((dTime = PrevData->Velocity_Linear.LEN()) <= 1.00000000000001);
		ASSERT(Velocity_Linear.LEN() <= 1.00000000000001);


		return true; //false если проблема
	}

	bool TAD_OnUpdate(SData* PrevData)
	{
		PrevData->Velocity_Linear.SET_AS_DELTA(Position, PrevData->Position);

		double dTime = TimePoint - PrevData->TimePoint;
		if (dTime > 0.0)
		{
			PrevData->Velocity_Linear /= dTime;
			if (PrevData->Velocity_Linear.LEN() > 1.0)
				PrevData->Velocity_Linear.SET_LENGHT_TO_1();
		}
		else
			PrevData->Velocity_Linear = PrevData->Velocity;

		Velocity_Linear = Velocity;

		ASSERT(PrevData->Velocity_Linear.LEN() <= 1.00000000000001);
		ASSERT(Velocity_Linear.LEN() <= 1.00000000000001);

		return true; //false если проблема
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


int ReadCalcParam(const char* in_Src, const char* in_Mask, SPos& data, double defValu, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(const char* in_Src, const char* in_Mask, int& data, int defValue, int inFlags = READPARAMS_USE_DEFS);
int ReadCalcParam(const char* in_Src, const char* in_Mask, double& data, double defValue, int inFlags = READPARAMS_USE_DEFS);
