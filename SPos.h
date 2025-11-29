

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
	inline double DOT(const SPos& in_DOT) { return X * in_DOT.X + Y * in_DOT.Y + Z * in_DOT.Z;	}

	//длина от 0,0,0
	inline double LEN() { return sqrt(X * X + Y * Y + Z * Z); }
	inline double LENPWR2() { return X * X + Y * Y + Z * Z; }
	inline bool IsZero() { return X == 0.0 && Y == 0.0 && Z == 0.0; }
	inline double LENPWR3() { double tmp = LEN();	return tmp * tmp * tmp; }

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

	inline double GetDistance(const SPos& Ptr1) { return sqrt(GetDistancePwr2(Ptr1)); }
	inline double GetDistancePwr2(const SPos& Ptr1)
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

	static int PRINT_AS_NULL(char* io_Str, int in_StrLen, int in_ValueDelim = '\t')
	{
		int ret = 0;
		if (io_Str)
		{
			for (int i = 0; i < 3 && in_StrLen - ret > 0; i++)
				if (in_StrLen - (ret += sprintf_s(io_Str + ret, in_StrLen - ret, "NULL")) > 0 && i < 2)
					io_Str[ret++] = in_ValueDelim;

			io_Str[ret] = 0;
		}
		return ret;
	}


	int PRINT(char* io_Str, int in_StrLen, int in_ValueDelim = '\t', const char* in_Format = "%g")
	{
		int ret = 0;
		if (io_Str)
		{
			if (in_Format && *in_Format)
			{
				double* pData = &X;

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
	double position;
	double Speed;
	double Accel3D;
	double AccelDeriv3D;
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

	void CLEAR()
	{
		position = { 0 };
		Speed = { 0 };
		Accel3D = { 0 };
		AccelDeriv3D = { 0 };
		AbsSpeed = 0.0;
		Density = 0.0;
		TimePoint = -9.0e99; // типа, это время
	};

	static int PRINT_AS_NULL(char* io_Str, int in_StrLen, int in_Flags, int in_ValueDelim = '\t', int in_SubValueDelim = '\t')
	{
		int ret = 0;
		if (io_Str)
		{
			const char* txtNULL = "NULL";
			int nOutput;
			if (in_StrLen - ret > 0 && (in_Flags & SDATA_POSITION))
			{
				in_Flags ^= SDATA_POSITION;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "position");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
						io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_SPEED))
			{
				in_Flags ^= SDATA_SPEED;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Speed");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCEL3D))
			{
				in_Flags ^= SDATA_ACCEL3D;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Accel3D");
				if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
				nOutput = SPos::PRINT_AS_NULL(io_Str + ret, in_StrLen - ret, in_SubValueDelim);
				if (in_StrLen - (ret += nOutput) > 0 && in_Flags)
					io_Str[ret++] = in_ValueDelim;
			}

			if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCELDERIV3D))
			{
				in_Flags ^= SDATA_ACCELDERIV3D;
				nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "AccelDeriv3D");
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

	int PRINT(char* io_Str, int in_StrLen, int in_Flags, int in_ValueDelim = '\t', int in_SubValueDelim = '\t', const char* in_Format = "%g")
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
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "position");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;

					nOutput = position.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_SPEED))
				{
					in_Flags ^= SDATA_SPEED;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Speed");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = Speed.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCEL3D))
				{
					in_Flags ^= SDATA_ACCEL3D;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "Accel3D");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = Accel3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
					if (in_StrLen - (ret += nOutput) > 0  && in_Flags)	io_Str[ret++] = in_ValueDelim;
				}

				if (in_StrLen - ret > 0 && (in_Flags & SDATA_ACCELDERIV3D))
				{
					in_Flags ^= SDATA_ACCELDERIV3D;
					nOutput = sprintf_s(io_Str + ret, in_StrLen - ret, "AccelDeriv3D");
					if (in_StrLen - (ret += nOutput) > 0) io_Str[ret++] = in_ValueDelim;
					nOutput = AccelDeriv3D.PRINT(io_Str + ret, in_StrLen - ret, in_SubValueDelim, in_Format);
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

