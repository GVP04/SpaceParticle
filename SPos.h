
class CSDataArray;
class CSpParticle;
struct SData;

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

	// Вычитание векторов
	void MINUS(const SPos& in_MINUS)
	{
		X -= in_MINUS.X;
		Y -= in_MINUS.Y;
		Z -= in_MINUS.Z;
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

	// Скалярное произведение
	double DOT(const SPos& in_DOT) { return X * in_DOT.X + Y * in_DOT.Y + Z * in_DOT.Z;	}

	//длина от 0,0,0
	double LEN() { return sqrt(X * X + Y * Y + Z * Z); }
	double LENPWR2() { return X * X + Y * Y + Z * Z; }
	bool IsZero() { return X == 0.0 && Y == 0.0 && Z == 0.0; }
	static double LENPWR2(const SPos& Ptr1) { return Ptr1.X * Ptr1.X + Ptr1.Y * Ptr1.Y + Ptr1.Z * Ptr1.Z; }
	double LENPWR3() { double tmp = LEN();	return tmp * tmp * tmp; }

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

	double GetDistance(const SPos& Ptr1) { return sqrt(GetDistancePwr2(Ptr1)); }
	double GetDistancePwr2(const SPos& Ptr1)
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

	void SET_AS_REFLECT(const SPos& Velocity, const SPos& Normal)
	{
		*this = Velocity;

		SPos n_norm = Normal;
		n_norm.NORMALIZE();

		// Вычисляем скалярное произведение
		double dotProduct = this->DOT(n_norm);

		// Рассчитываем отраженный вектор
		n_norm.MULT(dotProduct + dotProduct);
		MINUS(n_norm);
	}

	void SET_AS_REFLECT(const SPos& Velocity, const SPos& planeVelocity, const SPos& Normal)
	{
		*this = Velocity;
		MINUS(planeVelocity);

		SPos n_norm = Normal;
		n_norm.NORMALIZE();

		// Вычисляем скалярное произведение
		double dotProduct = this->DOT(n_norm);

		// Рассчитываем отраженный вектор

		n_norm.MULT(dotProduct + dotProduct);
		MINUS(n_norm);
		ADD(planeVelocity);
	}

	void REFLECT(const SPos& Normal)
	{
		// Нормализуем вектор нормали
		SPos n_norm = Normal;
		n_norm.NORMALIZE();

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
		n_norm.NORMALIZE();

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
		n_norm.NORMALIZE();

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
		n_norm.NORMALIZE();

		// Вычисляем проекцию на нормаль
		n_norm.MULT(DOT(n_norm));

		// Находим параллельную компоненту
		MINUS(n_norm);

		// Нормализуем и меняем направление
		NORMALIZE();
		INVERT();
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
	double position;
	double Speed;
	double Accel3D;
	double AccelDeriv3D;
	double Density;
	double TimePoint; // типа, это время
};


struct SData
{
	SPos position;
	SPos Speed;
	SPos Accel3D;
	SPos AccelDeriv3D;
	double AbsSpeed;
	double Density;
	double TimePoint; // типа, это время


	double GetRelativeSpeed(const SData& Ptr1, SPos* RSpeed3D) { return  GetRelativeSpeed(*this, Ptr1, RSpeed3D); }

	static double GetDistance(const SData& Ptr1, const SData& Ptr2) { return SPos::GetDistance(Ptr1.position, Ptr2.position); }
	static double GetDistancePwr2(const SData& Ptr1, const SData& Ptr2) { return SPos::GetDistancePwr2(Ptr1.position, Ptr2.position); }

	static double GetRelativeSpeed(const SData& Ptr1, const SData& Ptr2, SPos* RSpeed3D)
	{
		double ret;

		SPos distanceVector = Ptr2.position; 
		distanceVector.MINUS(Ptr1.position);
		SPos velocityVector = Ptr2.Speed; 
		velocityVector.MINUS(Ptr1.Speed);

		double distance = distanceVector.LEN();

		if (distance == 0)
			ret = velocityVector.LEN();
		else
			ret = distanceVector.DOT(velocityVector) / distance;

		if (RSpeed3D)
		{
			*RSpeed3D = distanceVector;
			RSpeed3D->SET_LENGHT((ret));
		}

		return ret;
	}
};

