/*
 * ░▒▓ APPARATUS ▓▒░
 * 
 * File: SubjectRecordCollection.cpp
 * Created: 2022-05-13 14:51:57
 * Author: Vladislav Dmitrievich Turbanov (vladislav@turbanov.ru)
 * ───────────────────────────────────────────────────────────────────
 * 
 * The Apparatus source code is for your internal usage only.
 * Redistribution of this file is strictly prohibited.
 * 
 * Community forums: https://talk.turbanov.ru
 * 
 * Copyright 2019 - 2023, SP Vladislav Dmitrievich Turbanov
 * Made in Russia, Moscow City, Chekhov City ♡
 */

#include "SubjectRecordCollection.h"

#include "Machine.h"


void
USubjectRecordCollection::Add(FSubjectHandle  Subject,
							  const EFlagmark FlagmarkMask/*=FM_AllUserLevel*/)
{
	check(Subject.IsValid());
	Subjects.Add(FSubjectRecord(Subject, FlagmarkMask));
}

void
USubjectRecordCollection::Reset(int32 NewSize/*=0*/)
{
	Subjects.Reset(NewSize);
}


void
USubjectRecordCollection::Empty(int32 Slack/*=0*/)
{
	Subjects.Empty(Slack);
}
