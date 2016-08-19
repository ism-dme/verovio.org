/////////////////////////////////////////////////////////////////////////////
// Name:        measure.h
// Author:      Laurent Pugin
// Created:     2012
// Copyright (c) Authors and others. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_MEASURE_H__
#define __VRV_MEASURE_H__

#include "aligner.h"
#include "atts_shared.h"
#include "barline.h"
#include "object.h"

namespace vrv {

class Ending;
class FloatingElement;
class ScoreDef;
class TimestampAttr;

//----------------------------------------------------------------------------
// Measure
//----------------------------------------------------------------------------

/**
 * This class represents a measure in a page-based score (Doc).
 * A Measure is contained in a Staff.
 * It contains Layer objects.
 * For internally simplication of processing, unmeasured music is contained in one single measure object
 */
class Measure : public Object, public AttCommon, public AttMeasureLog, public AttPointing {

public:
    /**
     * @name Constructors, destructors, and other standard methods
     * Reset method resets all attribute classes
     */
    ///@{
    Measure(bool measuredMusic = true, int logMeasureNb = -1);
    virtual ~Measure();
    virtual void Reset();
    virtual std::string GetClassName() const { return "Measure"; };
    virtual ClassId Is() const { return MEASURE; };
    ///@}

    /**
     * Return true if measured music (otherwise we have fake measures)
     */
    bool IsMeasuredMusic() const { return m_measuredMusic; };

    /**
     * @name Methods for adding allowed content
     */
    ///@{
    void AddStaff(Staff *staff);
    void AddFloatingElement(FloatingElement *element);
    ///@}

    /**
     * Add a TimestampAttr to the measure.
     * The TimestampAttr it not added as child of the measure but to the Measure::m_timestamps array.
     */
    void AddTimestamp(TimestampAttr *timestampAttr);

    /**
     * Return the index position of the measure in its system parent
     */
    int GetMeasureIdx() const { return Object::GetIdx(); };

    /**
     * @name Set and get the left and right barline types
     * This somehow conflicts with AttMeasureLog, which is transfered from and to the
     * Barline object when reading and writing MEI. See MeiInput::ReadMeiMeasure and
     * MeiOutput::ReadMeiMeasure
     * Alternatively, we could keep them in sync here:
     * data_BARRENDITION GetLeftBarLineType() { m_leftBarLine.SetRend(GetRight()); return m_leftBarLine.GetRend(); };
     * void SetLeftBarLineType(data_BARRENDITION type) { m_leftBarLine.SetRend(type); SetLeft(type); };
     */
    ///@{
    data_BARRENDITION GetLeftBarLineType() const { return m_leftBarLine.GetForm(); };
    void SetLeftBarLineType(data_BARRENDITION type) { m_leftBarLine.SetForm(type); };
    data_BARRENDITION GetRightBarLineType() const { return m_rightBarLine.GetForm(); };
    void SetRightBarLineType(data_BARRENDITION type) { m_rightBarLine.SetForm(type); };
    ///@}

    /**
     * @name Set and get the barlines.
     * Careful - the barlines are owned by the measure and will be destroyed by it.
     * This method should be used only for acessing them (e.g., when drawing) and
     * not for creating other measure objects.
     */
    ///@{
    BarLine *const GetLeftBarLine() { return &m_leftBarLine; };
    BarLine *const GetRightBarLine() { return &m_rightBarLine; };
    ///@}

    // int GetXRel() const;

    /**
     * Return the non-justifiable left margin for the measure
     */
    int GetNonJustifiableLeftMargin() const { return m_measureAligner.GetNonJustifiableMargin(); }

    /**
     * @name Return the X1 and X2 rel position of the right and left barLine
     */
    ///@{
    int GetLeftBarLineXRel() const;
    int GetLeftBarLineX1Rel() const;
    int GetLeftBarLineX2Rel() const;
    int GetRightBarLineXRel() const;
    int GetRightBarLineX1Rel() const;
    int GetRightBarLineX2Rel() const;
    ///@}

    /**
     * Return the width of the measure, including the barLine width
     */
    int GetWidth() const;

    /**
     * @name Setter and getter of the drawing scoreDef
     */
    ///@{
    ScoreDef *GetDrawingScoreDef() const { return m_drawingScoreDef; };
    void SetDrawingScoreDef(ScoreDef *drawingScoreDef);
    ///@}

    /**
     * @name Setter and getter of the drawing ending
     */
    ///@{
    Ending *GetDrawingEnding() const { return m_drawingEnding; }
    void SetDrawingEnding(Ending *ending) { m_drawingEnding = ending; }
    ///@}

    /*
     * Return the first staff of each staffGrp according to the scoreDef
     */
    std::vector<Staff *> GetFirstStaffGrpStaves(ScoreDef *scoreDef);

    //----------//
    // Functors //
    //----------//

    /**
     * Unset the initial scoreDef of each system and measure
     */
    virtual int UnsetCurrentScoreDef(FunctorParams *functorParams);

    /**
     * @name Reset the horizontal alignment
     */
    ///@{
    virtual int ResetHorizontalAlignment(FunctorParams *functorParams);
    ///@}

    /**
     * AlignHorizontally the content of a measure.
     */
    virtual int AlignHorizontally(FunctorParams *functorParams);

    /**
     * Align horizontally the content of a layer.
     */
    virtual int AlignHorizontallyEnd(FunctorParams *functorParams);

    /**
     * AlignVertically the content of a measure.
     */
    virtual int AlignVertically(FunctorParams *functorParams);

    /**
     * Correct the X alignment of grace notes once the content of a system has been aligned and laid out.
     * Special case that redirects the functor to the GraceAligner.
     */
    virtual int IntegrateBoundingBoxGraceXShift(FunctorParams *functorParams);

    /**
     * Correct the X alignment once the content of a system has been aligned and laid out.
     * Special case that redirects the functor to the MeasureAligner.
     */
    virtual int IntegrateBoundingBoxXShift(FunctorParams *functorParams);

    /**
     * Set the position of the Alignment.
     * Special case that redirects the functor to the MeasureAligner.
     */
    virtual int SetAlignmentXPos(FunctorParams *functorParams);

    /**
     * Lay out the X positions of the staff content looking that the bounding boxes.
     * The m_xShift is updated appropriately
     */
    virtual int SetBoundingBoxXShift(FunctorParams *functorParams);

    /**
     * Lay out the X positions of the staff content looking at the bounding boxes.
     * This is the Functor called at the end of the measure or a layer.
     */
    virtual int SetBoundingBoxXShiftEnd(FunctorParams *functorParams);

    /**
     * Align the measures by adjusting the m_drawingXRel position looking at the MeasureAligner.
     * This method also moves the end position of the measure according to the barLine width.
     */
    virtual int AlignMeasures(FunctorParams *functorParams);

    /**
     * Justify the X positions
     * Special case that redirects the functor to the MeasureAligner.
     */
    virtual int JustifyX(FunctorParams *functorParams);

    /**
     * Fill a page by adding systems with the appropriate length
     *
     */
    virtual int CastOffSystems(FunctorParams *functorParams);

    /**
     * Set the drawing position (m_drawingX and m_drawingY) values for objects
     */
    virtual int SetDrawingXY(FunctorParams *functorParams);

    /**
     * Reset the drawing values before calling PrepareDrawing after changes.
     */
    virtual int ResetDrawing(FunctorParams *functorParams);

    /**
     * See Object::FillStaffCurrentTimeSpanningEnd
     */
    virtual int FillStaffCurrentTimeSpanningEnd(FunctorParams *functorParams);

    /**
     * Functor for grouping FloatingElement by drawingGrpId
     */
    virtual int PrepareFloatingGrps(FunctorParams *functoParams);

    /**
     * See Object::PrepareTimeSpanning.
     */
    virtual int PrepareTimeSpanningEnd(FunctorParams *functorParams);

    /**
     * See Object::PrepareBoundaries.
     */
    virtual int PrepareBoundaries(FunctorParams *functorParams);

    /**
     * See Object::GenerateMIDI
     */
    virtual int GenerateMIDI(FunctorParams *functorParams);

    /**
     * See Object::GenerateMIDI
     */
    virtual int GenerateMIDIEnd(FunctorParams *functorParams);

    /**
     * See Object::CalcMaxMeasureDuration
     */
    virtual int CalcMaxMeasureDuration(FunctorParams *functorParams);

    /**
     * See Object::PrepareTimestamps.
     */
    virtual int PrepareTimestampsEnd(FunctorParams *functorParams);

public:
    /**
     * The X absolute position of the measure for facsimile (transcription) encodings.
     * This is the top left position of the measure.
     */
    int m_xAbs;
    /**
     * The X relative position of the measure.
     * It is used internally when calculating the layout and it is not stored in the file.
     */
    int m_drawingXRel;

    /**
     * The measure aligner that holds the x positions of the content of the measure
     */
    MeasureAligner m_measureAligner;

    TimestampAligner m_timestampAligner;

private:
    bool m_measuredMusic;

    /**
     * @name The measure barlines (left and right) used when drawing
     */
    ///@{
    BarLineAttr m_leftBarLine;
    BarLineAttr m_rightBarLine;
    ///@}

    /**
     * A pointer to the drawing ScoreDef instance. It is owned by the measure and added to a measure when a scoreDef
     * change before it required it. This include scoreDef elements before it but also clef changes within the previous
     * measure.
     */
    ScoreDef *m_drawingScoreDef;

    /**
     * A pointer to the ending to which the measure belongs. Set by PrepareBoundaries and passed to the System drawing
     * list
     * in DrawMeasure
     */
    Ending *m_drawingEnding;
};

} // namespace vrv

#endif
